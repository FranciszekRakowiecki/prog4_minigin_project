//
// Created by frane on 5/27/2026.
//

#ifndef PROG4MINIGINPROJECT_MAINMENUSTATE_H
#define PROG4MINIGINPROJECT_MAINMENUSTATE_H
#include "GameState.h"


namespace dae {
    class Scene;
}

class MainMenuState : public GameState {
    dae::Scene* m_Scene;
public:
    void Enter() override;
    void Exit() override;
    void Update() override {}
    void Render() override {}
};

#endif //PROG4MINIGINPROJECT_MAINMENUSTATE_H