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

class EnemyTankAI : public dae::Component {
public:
    int GetFlags() override;
    void Start() override;
    void Update() override;

    void SetEnemyType(EnemyType type);
    void SetLevelData(const LevelData* levelData, const glm::vec3& levelOrigin, float tileSize);
    void SetTargets(const std::vector<dae::GameObject*>& targets);

private:
    const LevelData* m_LevelData{nullptr};
    TankController* m_TankController{nullptr};
    std::vector<dae::GameObject*> m_Targets{};
    EnemyType m_EnemyType{EnemyType::BlueTank};
    glm::vec3 m_LevelOrigin{};
    float m_TileSize{16.0f};
    std::default_random_engine m_RandomGenerator{};

    void PickMove();
    bool TryHandleVisiblePlayer();
    bool IsTargetVisible(dae::GameObject* target, Direction& directionToTarget) const;
    bool HasClearLine(uint16_t fromX, uint16_t fromY, uint16_t toX, uint16_t toY) const;
    bool TryGetTargetTile(dae::GameObject* target, uint16_t& tileX, uint16_t& tileY) const;
    std::vector<Direction> GetPatrolOptions() const;
    static Direction TurnLeft(Direction direction);
    static Direction TurnRight(Direction direction);
    static Direction Flip(Direction direction);
};


#endif //PROG4MINIGINPROJECT_ENEMYTANKAI_H
