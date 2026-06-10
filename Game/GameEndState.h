//
// Created by frane on 6/10/2026.
//

#ifndef PROG4MINIGINPROJECT_GAMEENDSTATE_H
#define PROG4MINIGINPROJECT_GAMEENDSTATE_H
#include "GameState.h"


namespace dae {
    class Scene;
}

class GameEndState : public GameState {
public:
    void Update() override;

    void Render() override;

    void Enter() override;

    void Exit() override;

private:
    dae::Scene* m_Scene{nullptr};
};


#endif //PROG4MINIGINPROJECT_GAMEENDSTATE_H
