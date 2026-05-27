//
// Created by frane on 5/27/2026.
//

#ifndef PROG4MINIGINPROJECT_SINGLEPLAYERSTATE_H
#define PROG4MINIGINPROJECT_SINGLEPLAYERSTATE_H
#include "GameState.h"


class SinglePlayerState : public GameState {
public:
    void Enter() override;
    void Update() override {}
    void Render() override {}
};


#endif //PROG4MINIGINPROJECT_SINGLEPLAYERSTATE_H