//
// Created by frane on 6/9/2026.
//

#include "TankController.h"

#include "GameObject.h"
#include "GameTime.h"
#include "PlayerInputHandler.h"

int TankController::GetFlags() {
    return COMPONENT_HAS_UPDATE;
}

void TankController::Update() {
    GetParent()->transform.SetRotation(dae::GameTime::GetInstance().GetTime());
}

void TankController::SetPlayerInput(const PlayerInputHandler *handler) {
    m_PlayerInput = handler;
}
