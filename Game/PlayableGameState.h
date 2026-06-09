//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_PLAYABLEGAMESTATE_H
#define PROG4MINIGINPROJECT_PLAYABLEGAMESTATE_H
#include "GameState.h"


namespace dae {
    struct InputKey;
}

struct LevelData;

namespace dae {
    class Scene;
}

class PlayableGameState : public GameState {
public:
    void LoadLevel0(dae::Scene* scene);
    void LoadLevel1(dae::Scene* scene);
    void LoadLevel2(dae::Scene* scene);

    void LoadLevelWithData(dae::Scene* scene, LevelData* data);

    virtual void OnLevelSkip() {}
};


#endif //PROG4MINIGINPROJECT_PLAYABLEGAMESTATE_H