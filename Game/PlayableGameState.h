//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_PLAYABLEGAMESTATE_H
#define PROG4MINIGINPROJECT_PLAYABLEGAMESTATE_H

#include "GameState.h"

#include <cstdint>
#include <vector>
#include <glm/vec3.hpp>

namespace dae {
    class GameObject;
    class Scene;
}

class PlayerInputHandler;
class ProjectileSystem;
struct LevelData;
struct LevelSpawnPoint;

class PlayableGameState : public GameState {
public:
    void Enter() override;
    void Exit() override;
    void Update() override {}
    void Render() override {}

    virtual void OnLevelSkip();

    LevelData* GetCurrentLevelData() const;
    uint32_t GetCurrentLevelIndex() const;

protected:
    void LoadLevel(uint32_t index);
    void LoadLevel0();
    void LoadLevel1();
    void LoadLevel2();
    void LoadLevelWithData(LevelData* data, uint32_t index);

    dae::GameObject* SpawnPlayer(const PlayerInputHandler* handler);
    void SpawnEnemies();
    void RespawnPlayer(dae::GameObject* player);

    glm::vec3 TileToWorld(uint32_t x, uint32_t y) const;
    dae::Scene* GetScene() const;
    ProjectileSystem* GetProjectileSystem() const;

    virtual void OnLevelLoad(LevelData*, uint32_t) {}

private:
    void UnloadCurrentScene();
    const LevelSpawnPoint* FindSpawnPoint(uint32_t playerIndex) const;

    dae::Scene* m_Scene{nullptr};
    LevelData* m_CurrentLevel{nullptr};
    uint32_t m_CurrentLevelIndex{0};
    std::vector<dae::GameObject*> m_PlayerObjects{};
    std::vector<dae::GameObject*> m_EnemyObjects{};
    ProjectileSystem* m_ProjectileSystem{nullptr};
};

#endif //PROG4MINIGINPROJECT_PLAYABLEGAMESTATE_H
