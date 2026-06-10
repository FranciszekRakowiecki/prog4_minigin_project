//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_PLAYABLEGAMESTATE_H
#define PROG4MINIGINPROJECT_PLAYABLEGAMESTATE_H
#include "GameState.h"
#include <glm/vec3.hpp>

#include "Component.h"


namespace dae {
    class GameObject;
    class Scene;
    struct InputKey;
}

class PlayerInputHandler;
struct LevelData;

class PlayableGameState : public GameState {
public:
    void LoadLevel0(dae::Scene* scene);
    void LoadLevel1(dae::Scene* scene);
    void LoadLevel2(dae::Scene* scene);

    void LoadLevelWithData(dae::Scene* scene, const LevelData* data, uint32_t index);

    void SpawnPlayer(dae::Scene* scene, const LevelData* data, const PlayerInputHandler* handler);
    void SpawnEnemies(dae::Scene* scene, const LevelData* data);

    void RespawnPlayer(dae::GameObject* player, const LevelData* data);

    glm::vec3 TiletoWorld(uint32_t x, uint32_t y, const LevelData* data);

    virtual void OnLevelSkip() {}
    virtual void OnLevelLoad(dae::Scene* scene, const LevelData* data, uint32_t index) {}
    virtual void UnloadCurrentScene() {}

    const LevelData* GetCurrentLevelData() const;
    uint32_t GetCurrentLevelIndex() const;
private:
    const LevelData* m_CurrentLevel{nullptr};
    uint32_t m_CurrentLevelIndex{0};
};


#endif //PROG4MINIGINPROJECT_PLAYABLEGAMESTATE_H