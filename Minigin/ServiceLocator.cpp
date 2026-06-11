//
// Created by frane on 4/29/2026.
//

#include "ServiceLocator.h"

#include <iostream>

ServiceLocator::ServiceLocator() {
}

void ServiceLocator::Init() {
    if (!m_SoundSystem->Init()) {
        std::cerr << "Failed to initialize sound system" << std::endl;
        return;
    }
}

void ServiceLocator::Shutdown() {
    m_SoundSystem->Shutdown();
}

void ServiceLocator::update() {
    m_SoundSystem->update();
}

void ServiceLocator::setSoundSystem(std::unique_ptr<SoundSystem>&& s) {
    if (m_SoundSystem) {
        m_SoundSystem->Shutdown();
    }

    m_SoundSystem = std::move(s);
    if (m_SoundSystem == nullptr) {
        m_SoundSystem = std::make_unique<NullSoundSystem>();
    }

    if (!m_SoundSystem->Init()) {
        std::cerr << "Failed to initialize sound system" << std::endl;
        m_SoundSystem = std::make_unique<NullSoundSystem>();
        m_SoundSystem->Init();
    }
}
