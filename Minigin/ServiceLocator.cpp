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
