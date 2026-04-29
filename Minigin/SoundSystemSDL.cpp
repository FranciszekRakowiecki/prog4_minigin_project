//
// Created by frane on 4/29/2026.
//

#include "SoundSystemSDL.h"

#include <algorithm>
#include <iostream>
#include <queue>

#include "SDL3/SDL.h"
#include "SDL3/SDL_audio.h"

class SoundSystemSDL::Impl {
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

    void popAndPlayNextSound();
    bool hasNext();

    std::vector<Sound*> sounds;
    std::vector<SoundInstance> soundInstances;
    std::vector<float> mixBuffer;
    SDL_AudioStream* m_Stream{};
    SDL_AudioSpec audioSpec{};

    std::queue<SoundPlayRequest> soundPlayRequests;

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

void SoundSystemSDL::Impl::popAndPlayNextSound() {
    SoundPlayRequest soundPlayRequest = soundPlayRequests.front();
    soundPlayRequests.pop();
    Sound* sound = sounds[soundPlayRequest.id];

    if (!sound) {
        return;
    }

    SoundInstance soundInstance{};
    soundInstance.sound = sound;
    soundInstance.position = 0;
    soundInstance.volume = soundPlayRequest.volume;
    soundInstance.isDone = false;

    soundInstances.emplace_back(soundInstance);
}

bool SoundSystemSDL::Impl::hasNext() {
    return !soundPlayRequests.empty();
}

SoundSystemSDL::Impl::Impl() {
    audioSpec = {};
}

void SoundSystemSDL::Impl::playSound(SoundId id, float volume) {
    if (id < 0)
        return;

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
    return true;
}

void SoundSystemSDL::Impl::Shutdown() {
    if (m_Stream) {
        SDL_DestroyAudioStream(m_Stream);
        m_Stream = nullptr;
    }

    for (Sound* sound : sounds) {
        delete sound;
    }

    sounds.clear();

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

// Returns -1 if no sound found.
SoundId SoundSystemSDL::Impl::registerSound(const std::string &path) {
    Sound* sound = loadSound(path);
    if (sound == nullptr) {
        std::cerr << "Failed to load sound: " << path << std::endl;
        return SoundId(-1);
    }

    sounds.emplace_back(sound);
    return SoundId(sounds.size() - 1);
}

void SoundSystemSDL::Impl::update() {
    if (!m_Stream) {
        return;
    }

    const int bytesQueued = SDL_GetAudioStreamQueued(m_Stream);

    if (bytesQueued > maxQueuedBytes) {
        return;
    }

    // here is the request queue setup as requested ig
    while (hasNext())
        popAndPlayNextSound();

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