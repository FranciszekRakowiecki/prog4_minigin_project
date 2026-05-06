//
// Created by frane on 5/6/2026.
//

#include "TextureSelector.h"

dae::Texture2D * PlayerTextureSelector::GetShootingTexture() {
    return m_PlayerTextures[int(m_PlayerMovementState) * 2 + (m_IsShooting ? 1 : 0)];
}

void PlayerTextureSelector::Start() {

}

dae::Texture2D * PlayerTextureSelector::GetPlayerTexture() {
    return GetShootingTexture();
}

void PlayerTextureSelector::SetPlayerInfo(int x, int y, bool isShooting) {
    x = std::clamp(x, -1, 1);
    y = std::clamp(y, -1, 1);

    m_IsShooting = isShooting;

    if (x == 0 && y == 0) return;

    switch (x) {
        case -1:
            m_PlayerMovementState = PlayerMovementState::LEFT;
            return;
        case 1:
            m_PlayerMovementState = PlayerMovementState::RIGHT;
            return;
    }

    switch (y) {
        case -1:
            m_PlayerMovementState = PlayerMovementState::DOWN;
            return;
        case 1:
            m_PlayerMovementState = PlayerMovementState::UP;
            return;
    }
}

void PlayerTextureSelector::Update() {

}
