//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_ENEMYTANKAI_H
#define PROG4MINIGINPROJECT_ENEMYTANKAI_H

#include "Component.h"
#include "LevelLoader.h"

#include <random>
#include <vector>

#include <glm/vec3.hpp>

class TankController;

namespace dae {
    class GameObject;
}

class PlayableGameState;

class EnemyTankAI : public dae::Component {
public:
    int GetFlags() override;
    void Start() override;
    void Update() override;

    void SetEnemyType(EnemyType type);
    void SetLevelData(const LevelData* levelData, const glm::vec3& levelOrigin, float tileSize);
    void SetHealth(int health);
    int GetHealth() const;
    void TakeDamage(int damage = 1);
    void SetShootDelay(float delay);
    int GetScoreValue() const;

private:
    const LevelData* m_LevelData{nullptr};
    dae::Reference<TankController> m_TankController{};
    EnemyType m_EnemyType{EnemyType::BlueTank};
    glm::vec3 m_LevelOrigin{};
    float m_TileSize{16.0f};
    int m_Health{3};
    float m_LastShot{0.0f};
    float m_ShootDelay{0.75f};
    std::default_random_engine m_RandomGenerator{};

    void PickMove();
    bool TryHandlePlayerCollision();
    bool TryHandleVisiblePlayer();
    bool IsOverlapping(dae::GameObject* target) const;
    bool IsTargetVisible(dae::GameObject* target, Direction& directionToTarget) const;
    bool HasClearLine(uint16_t fromX, uint16_t fromY, uint16_t toX, uint16_t toY) const;
    bool TryGetTargetTile(dae::GameObject* target, uint16_t& tileX, uint16_t& tileY) const;
    std::vector<Direction> GetPatrolOptions() const;
    PlayableGameState* GetPlayableGameState() const;
    static Direction TurnLeft(Direction direction);
    static Direction TurnRight(Direction direction);
    static Direction Flip(Direction direction);
};


#endif //PROG4MINIGINPROJECT_ENEMYTANKAI_H
