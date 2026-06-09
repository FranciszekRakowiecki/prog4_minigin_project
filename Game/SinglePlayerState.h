//
// Created by frane on 5/27/2026.
//

#ifndef PROG4MINIGINPROJECT_SINGLEPLAYERSTATE_H
#define PROG4MINIGINPROJECT_SINGLEPLAYERSTATE_H
#include "GameState.h"
#include "PlayableGameState.h"


namespace dae {
    class Scene;
}

class SinglePlayerState : public PlayableGameState {
public:
    void Enter() override;
    void Exit() override;
    void Update() override {}
    void Render() override {}

private:
    dae::Scene* m_Scene;
};


#endif //PROG4MINIGINPROJECT_SINGLEPLAYERSTATE_H