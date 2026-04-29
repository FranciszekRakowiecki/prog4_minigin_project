//
// Created by frane on 4/29/2026.
//

#ifndef MINIGIN_SOUNDSYSTEM_H
#define MINIGIN_SOUNDSYSTEM_H
#include <memory>
#include <string>
#include <vector>

struct Sound {
    std::vector<float> samples;
    int channels = 2;
};

using SoundId = int32_t;

class SoundSystem {
public:
    SoundSystem() = default;

    virtual ~SoundSystem() = default;

    virtual void playSound(SoundId id, float volume) = 0;

    virtual bool Init() = 0;
    virtual void Shutdown() = 0;

    virtual void update() = 0;

    virtual SoundId registerSound(const std::string& path) = 0;

    struct SoundInstance {
        Sound* sound;
        size_t position;
        float volume;
        bool isDone;
    };
};


#endif //MINIGIN_SOUNDSYSTEM_H