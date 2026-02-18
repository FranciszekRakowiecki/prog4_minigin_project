//
// Created by frane on 2/18/2026.
//
#pragma once

#ifndef MINIGIN_GAMETIME_H
#define MINIGIN_GAMETIME_H
#include <chrono>

#include "Singleton.h"


namespace dae {
    class GameTime : public dae::Singleton<GameTime> {

        static double getTimeSinceEpoch();

        float m_DeltaTime{};

        double m_StartTime{getTimeSinceEpoch()};

        void SetDeltaTime(float deltaTime);

    public:
        float GetDeltaTime() const;
        float GetTime() const;

        friend class Minigin;
    };
}

#endif //MINIGIN_GAMETIME_H