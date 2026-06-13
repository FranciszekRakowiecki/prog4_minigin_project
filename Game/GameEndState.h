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
    bool HasPendingHighScore() const;
    bool IsAnyPlayerSubmitting() const;

    dae::Scene* m_Scene{nullptr};
    float m_ReturnDelay{0.75f};
    float m_ReturnDelayTimer{0.75f};
};


#endif //PROG4MINIGINPROJECT_GAMEENDSTATE_H
