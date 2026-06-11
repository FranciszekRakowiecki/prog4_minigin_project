//
// Created by frane on 5/27/2026.
//

#ifndef PROG4MINIGINPROJECT_COOPPLAYERSTATE_H
#define PROG4MINIGINPROJECT_COOPPLAYERSTATE_H
#include "GameState.h"
#include "PlayableGameState.h"


class CoopPlayerState : public PlayableGameState {
protected:
    void OnLevelLoad(LevelData *, uint32_t) override;
};


#endif //PROG4MINIGINPROJECT_COOPPLAYERSTATE_H