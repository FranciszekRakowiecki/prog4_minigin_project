//
// Created by frane on 5/27/2026.
//

#ifndef PROG4MINIGINPROJECT_VERSUSPLAYERSTATE_H
#define PROG4MINIGINPROJECT_VERSUSPLAYERSTATE_H
#include "GameState.h"
#include "PlayableGameState.h"


class VersusPlayerState : public PlayableGameState {
protected:
    void OnLevelLoad(LevelData *, uint32_t) override;
};


#endif //PROG4MINIGINPROJECT_VERSUSPLAYERSTATE_H