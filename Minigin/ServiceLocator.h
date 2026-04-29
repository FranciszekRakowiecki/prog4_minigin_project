//
// Created by frane on 4/29/2026.
//

#ifndef MINIGIN_SERVICELOCATOR_H
#define MINIGIN_SERVICELOCATOR_H
#include <memory>

#include "Singleton.h"
#include "SoundSystem.h"

class NullSoundSystem final : public SoundSystem {
public:
    NullSoundSystem() = default;
    ~NullSoundSystem() override {}

    void playSound(SoundId, float) override {}

    bool Init() override { return true; }

    void Shutdown() override { }

    SoundId registerSound(const std::string &) override { return SoundId(-1); }

    void update() override {}
};

class ServiceLocator final : public dae::Singleton<ServiceLocator> {
private:
    std::unique_ptr<SoundSystem> m_SoundSystem = std::make_unique<NullSoundSystem>();
    ServiceLocator();
public:
    void Init();
    void Shutdown();
    void update();
    // Makes sure the sound system isn't nullptr
    void setSoundSystem(std::unique_ptr<SoundSystem>&& s) {
        m_SoundSystem = std::move(s);
        if (m_SoundSystem == nullptr)
            m_SoundSystem = std::make_unique<NullSoundSystem>();
    }
    SoundSystem& getSoundSystem() const { return *m_SoundSystem; }

    friend class Singleton<ServiceLocator>;
};


#endif //MINIGIN_SERVICELOCATOR_H