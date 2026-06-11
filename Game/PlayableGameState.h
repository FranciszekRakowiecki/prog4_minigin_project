//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_PLAYABLEGAMESTATE_H
#define PROG4MINIGINPROJECT_PLAYABLEGAMESTATE_H

#include "GameState.h"

#include <cstdint>
#include <random>
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
    void Update() override;
    void Render() override {}

    virtual void OnLevelSkip();

    LevelData* GetCurrentLevelData() const;
    uint32_t GetCurrentLevelIndex() const;
    const std::vector<dae::GameObject*>& GetPlayerObjects() const;
    const std::vector<dae::GameObject*>& GetEnemyObjects() const;

    ProjectileSystem* GetProjectileSystem() const;
    void RespawnPlayerAtRandomSpawn(dae::GameObject* player, bool loseLife);

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

    virtual void OnLevelLoad(LevelData*, uint32_t) {}

private:
    void UnloadCurrentScene();
    const LevelSpawnPoint* FindSpawnPoint(uint32_t playerIndex) const;
    void CheckLevelCompletion();
    void RemoveDestroyedObjects(std::vector<dae::GameObject*>& objects);
    bool AreAllPlayersDead() const;
    bool AreAllEnemiesDestroyed() const;
    void CompleteLevel();
    void EndGame();

    dae::Scene* m_Scene{nullptr};
    LevelData* m_CurrentLevel{nullptr};
    uint32_t m_CurrentLevelIndex{0};
    std::vector<dae::GameObject*> m_PlayerObjects{};
    std::vector<dae::GameObject*> m_EnemyObjects{};
    ProjectileSystem* m_ProjectileSystem{nullptr};
    bool m_IsCompletingLevel{false};
    std::default_random_engine m_RandomGenerator{};
};

#endif //PROG4MINIGINPROJECT_PLAYABLEGAMESTATE_H
