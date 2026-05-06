//
// Created by frane on 5/6/2026.
//

#ifndef PROG4MINIGINPROJECT_TEXTURESELECTOR_H
#define PROG4MINIGINPROJECT_TEXTURESELECTOR_H
#include "Component.h"

namespace dae {
    class Texture2D;
}

enum class PlayerMovementState {
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class PlayerTextureSelector : public dae::Component {
    std::vector<dae::Texture2D*> m_PlayerTextures{};
    PlayerMovementState m_PlayerMovementState{PlayerMovementState::LEFT};
    bool m_IsShooting{0};

    dae::Texture2D* GetShootingTexture();

public:
    void Start() override;
    dae::Texture2D* GetPlayerTexture();
    void SetPlayerInfo(int x, int y, bool isShooting); // the game is grid based
    void Update() override;
    int GetFlags() override { return COMPONENT_HAS_UPDATE; }
};


#endif //PROG4MINIGINPROJECT_TEXTURESELECTOR_H
