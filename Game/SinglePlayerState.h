//
// Created by frane on 5/27/2026.
//

#ifndef PROG4MINIGINPROJECT_SINGLEPLAYERSTATE_H
#define PROG4MINIGINPROJECT_SINGLEPLAYERSTATE_H
#include "PlayableGameState.h"


struct LevelData;

class SinglePlayerState : public PlayableGameState {
protected:
    void OnLevelLoad(LevelData* data, uint32_t index) override;
};


#endif //PROG4MINIGINPROJECT_SINGLEPLAYERSTATE_H
