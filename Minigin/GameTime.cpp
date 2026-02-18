//
// Created by frane on 2/18/2026.
//

#include "GameTime.h"

void dae::GameTime::SetDeltaTime(float deltaTime) {
    this->m_DeltaTime = deltaTime;
}

double dae::GameTime::getTimeSinceEpoch()  {
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

float dae::GameTime::GetDeltaTime() const {
    return m_DeltaTime;
}

float dae::GameTime::GetTime() const {
    return float(getTimeSinceEpoch() - m_StartTime);
}
