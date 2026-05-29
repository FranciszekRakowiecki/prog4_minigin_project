//
// Created by frane on 4/29/2026.
//

#include "SoundSystemSDL.h"

#include <algorithm>
#include <iostream>
#include <mutex>
#include <queue>

#ifndef __EMSCRIPTEN__
#include <atomic>
#include <chrono>
#include <thread>
#endif

#include "SDL3/SDL.h"
#include "SDL3/SDL_audio.h"

class SoundSystemSDL::Impl {
#ifndef __EMSCRIPTEN__
    struct SoundLoadRequest {
        SoundId id;
        std::string path;
    };
#endif

    struct SoundPlayRequest {
        SoundId id;
        float volume;
    };
public:
    Impl();

    void playSound(SoundId id, float volume);

    bool Init();

    void Shutdown();

    void update();

    SoundId registerSound(const std::string &path);

    Sound* loadSound(const std::string& path);

#ifndef __EMSCRIPTEN__
    void startLoader();
    void stopLoader();
    void loaderLoop();
    void queueSoundLoad(SoundId id, const std::string& path);
#endif
    Sound* getSound(SoundId id);
    bool isSoundLoadFinished(SoundId id);
    void processPlayRequests();

    std::vector<Sound*> sounds;
    std::vector<bool> soundLoadFinished;
    std::vector<SoundInstance> soundInstances;
    std::vector<float> mixBuffer;
    SDL_AudioStream* m_Stream{};
    SDL_AudioSpec audioSpec{};

    std::queue<SoundPlayRequest> soundPlayRequests;
#ifndef __EMSCRIPTEN__
    std::queue<SoundLoadRequest> soundLoadRequests;
#endif

    std::mutex soundMutex;
    std::mutex playQueueMutex;
#ifndef __EMSCRIPTEN__
    std::mutex loadQueueMutex;
    std::thread loaderThread;
    std::atomic_bool loaderRunning{false};
#endif

    constexpr static int maxQueuedBytes = 48000 * 2 * sizeof(float) / 4;
};

void SoundSystemSDL::playSound(SoundId id, float volume) {
    m_Impl->playSound(id, volume);
}

bool SoundSystemSDL::Init() {
    return m_Impl->Init();
}

void SoundSystemSDL::Shutdown() {
    m_Impl->Shutdown();
}

SoundId SoundSystemSDL::registerSound(const std::string &path) {
    return m_Impl->registerSound(path);
}

SoundSystemSDL::SoundSystemSDL() {
    m_Impl = std::make_unique<Impl>();

}

SoundSystemSDL::~SoundSystemSDL() {
}

void SoundSystemSDL::update() {
    m_Impl->update();
}

Sound * SoundSystemSDL::Impl::loadSound(const std::string &path) {
    SDL_AudioSpec sourceSpec{};
    Uint8* sourceData = nullptr;
    Uint32 sourceLength = 0;

    if (!SDL_LoadWAV(path.c_str(), &sourceSpec, &sourceData, &sourceLength)) {
        std::cerr << "Failed to load WAV: " << path << '\n';
        std::cerr << SDL_GetError() << '\n';
        return nullptr;
    }

    Uint8* convertedData = nullptr;
    int convertedLength = 0;

    const bool success = SDL_ConvertAudioSamples(&sourceSpec,sourceData, sourceLength, &audioSpec, &convertedData, &convertedLength);

    SDL_free(sourceData);

    if (!success) {
        std::cerr << "Failed to convert WAV: " << SDL_GetError() << '\n';
        return nullptr;
    }

    Sound* sound = new Sound();

    const int sampleCount = convertedLength / sizeof(float);
    const float* samples = reinterpret_cast<float*>(convertedData);

    sound->samples.assign(samples, samples + sampleCount);

    SDL_free(convertedData);

    return sound;
}

#ifndef __EMSCRIPTEN__
void SoundSystemSDL::Impl::startLoader() {
    loaderRunning = true;
    loaderThread = std::thread(&Impl::loaderLoop, this);
}

void SoundSystemSDL::Impl::stopLoader() {
    loaderRunning = false;

    if (loaderThread.joinable()) {
        loaderThread.join();
    }
}

void SoundSystemSDL::Impl::loaderLoop() {
    while (loaderRunning) {
        SoundLoadRequest request{};
        bool hasRequest{false};

        {
            std::lock_guard lock(loadQueueMutex);
            if (!soundLoadRequests.empty()) {
                request = soundLoadRequests.front();
                soundLoadRequests.pop();
                hasRequest = true;
            }
        }

        if (!hasRequest) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        Sound* loadedSound = loadSound(request.path);

        std::lock_guard lock(soundMutex);

        if (loadedSound == nullptr) {
            std::cerr << "Failed to load sound: " << request.path << std::endl;
            const auto index = static_cast<size_t>(request.id);
            if (request.id >= 0 && index < soundLoadFinished.size()) {
                soundLoadFinished[index] = true;
            }
            continue;
        }

        const uint32_t index = uint32_t(request.id);
        if (request.id >= 0 && index < sounds.size()) {
            delete sounds[index];
            sounds[index] = loadedSound;
            soundLoadFinished[index] = true;
        } else {
            delete loadedSound;
        }
    }
}

void SoundSystemSDL::Impl::queueSoundLoad(SoundId id, const std::string& path) {
    std::lock_guard lock(loadQueueMutex);
    soundLoadRequests.push({id, path});
}
#endif

Sound* SoundSystemSDL::Impl::getSound(SoundId id) {
    if (id < 0) {
        return nullptr;
    }

    std::lock_guard lock(soundMutex);
    const auto index = static_cast<size_t>(id);
    if (index >= sounds.size()) {
        return nullptr;
    }

    return sounds[index];
}

bool SoundSystemSDL::Impl::isSoundLoadFinished(SoundId id) {
    if (id < 0) {
        return true;
    }

    std::lock_guard lock(soundMutex);
    const auto index = static_cast<size_t>(id);
    if (index >= soundLoadFinished.size()) {
        return true;
    }

    return soundLoadFinished[index];
}

void SoundSystemSDL::Impl::processPlayRequests() {
    std::queue<SoundPlayRequest> pendingRequests;

    {
        std::lock_guard lock(playQueueMutex);
        std::swap(pendingRequests, soundPlayRequests);
    }

    while (!pendingRequests.empty()) {
        SoundPlayRequest soundPlayRequest = pendingRequests.front();
        pendingRequests.pop();

        Sound* sound = getSound(soundPlayRequest.id);
        if (!sound) {
            if (!isSoundLoadFinished(soundPlayRequest.id)) {
                std::lock_guard lock(playQueueMutex);
                soundPlayRequests.push(soundPlayRequest);
            }
            continue;
        }

        SoundInstance soundInstance{};
        soundInstance.sound = sound;
        soundInstance.position = 0;
        soundInstance.volume = soundPlayRequest.volume;
        soundInstance.isDone = false;

        soundInstances.emplace_back(soundInstance);
    }
}

SoundSystemSDL::Impl::Impl() {
    audioSpec = {};
}

void SoundSystemSDL::Impl::playSound(SoundId id, float volume) {
    if (id < 0)
        return;

    std::lock_guard lock(playQueueMutex);
    soundPlayRequests.push({ id, volume });
}

bool SoundSystemSDL::Impl::Init() {
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        std::cerr << "SDL audio init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    audioSpec.format = SDL_AUDIO_F32;
    audioSpec.channels = 2;
    audioSpec.freq = 48000;

    m_Stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&audioSpec,nullptr,nullptr);

    if (!m_Stream) {
        std::cerr << "Failed to open audio stream: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_ResumeAudioStreamDevice(m_Stream);
#ifndef __EMSCRIPTEN__
    startLoader();
#endif
    return true;
}

void SoundSystemSDL::Impl::Shutdown() {
#ifndef __EMSCRIPTEN__
    stopLoader();
#endif

    if (m_Stream) {
        SDL_DestroyAudioStream(m_Stream);
        m_Stream = nullptr;
    }

    for (Sound* sound : sounds) {
        delete sound;
    }

    sounds.clear();
    soundLoadFinished.clear();

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

// Returns an id immediately. On desktop builds, the actual WAV load is queued on
// the background loader thread.
SoundId SoundSystemSDL::Impl::registerSound(const std::string &path) {
    SoundId id{};
    {
        std::lock_guard lock(soundMutex);
        sounds.emplace_back(nullptr);
        soundLoadFinished.emplace_back(false);
        id = SoundId(sounds.size() - 1);
    }

#ifndef __EMSCRIPTEN__
    queueSoundLoad(id, path);
#else
    Sound* sound = loadSound(path);
    std::lock_guard lock(soundMutex);
    const auto index = static_cast<size_t>(id);
    sounds[index] = sound;
    soundLoadFinished[index] = true;
#endif
    return id;
}

void SoundSystemSDL::Impl::update() {
    if (!m_Stream) {
        return;
    }

    const int bytesQueued = SDL_GetAudioStreamQueued(m_Stream);

    if (bytesQueued > maxQueuedBytes) {
        return;
    }

    processPlayRequests();

    const int framesToMix = 512;
    const int channels = audioSpec.channels;
    const int samplesToMix = framesToMix * channels;

    mixBuffer.clear();
    mixBuffer.resize(samplesToMix, 0.0f);

    for (SoundInstance& playingSound : soundInstances) {
        if (playingSound.isDone || !playingSound.sound) {
            continue;
        }

        Sound* sound = playingSound.sound;

        for (int sampleIndex = 0; sampleIndex < samplesToMix; ++sampleIndex) {
            if (playingSound.position >= sound->samples.size()) {
                playingSound.isDone = true;
                break;
            }

            mixBuffer[sampleIndex] +=
                sound->samples[playingSound.position] * playingSound.volume;

            playingSound.position++;
        }
    }

    for (float& sample : mixBuffer) {
        sample = std::clamp(sample, -1.0f, 1.0f);
    }

    if (!SDL_PutAudioStreamData(m_Stream, mixBuffer.data(), uint32_t(mixBuffer.size()) * sizeof(float))) {
        std::cerr << "Failed to queue mixed audio: " << SDL_GetError() << '\n';
    }

    soundInstances.erase(std::remove_if(soundInstances.begin(),soundInstances.end(),[](const SoundInstance& sound) {return sound.isDone;}),soundInstances.end());
}
