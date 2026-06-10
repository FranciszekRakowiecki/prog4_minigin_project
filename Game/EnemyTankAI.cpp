//
// Created by frane on 6/9/2026.
//

#include "EnemyTankAI.h"

#include <cstddef>

#include "GameObject.h"
#include "TankController.h"

int EnemyTankAI::GetFlags() {
    return COMPONENT_HAS_UPDATE;
}

void EnemyTankAI::Start() {
    dae::Reference<TankController> controller = GetParent()->GetComponent<TankController>();
    m_TankController = controller.get();
}

void EnemyTankAI::Update() {
    if (m_LevelData == nullptr || m_TankController == nullptr || m_TankController->IsMoving()) {
        return;
    }

    if (TryHandleVisiblePlayer()) {
        return;
    }

    PickMove();
}

void EnemyTankAI::SetEnemyType(EnemyType type) {
    m_EnemyType = type;
}

void EnemyTankAI::SetLevelData(const LevelData* levelData, const glm::vec3& levelOrigin, float tileSize) {
    m_LevelData = levelData;
    m_LevelOrigin = levelOrigin;
    m_TileSize = tileSize;
}

void EnemyTankAI::SetTargets(const std::vector<dae::GameObject*>& targets) {
    m_Targets = targets;
}

void EnemyTankAI::PickMove() {
    const std::vector<Direction> options = GetPatrolOptions();
    if (options.empty()) {

        return;
    }

    std::uniform_int_distribution<std::size_t> distribution{0, options.size() - 1};
    m_TankController->TryMove(options[distribution(m_RandomGenerator)]);
}

bool EnemyTankAI::TryHandleVisiblePlayer() {
    for (dae::GameObject* target : m_Targets) {
        uint16_t targetX{};
        uint16_t targetY{};
        if (TryGetTargetTile(target, targetX, targetY)
            && targetX == m_TankController->GetCurrentTileX()
            && targetY == m_TankController->GetCurrentTileY()) {
            return true;
        }

        Direction directionToTarget{};
        if (!IsTargetVisible(target, directionToTarget)) {
            continue;
        }

        if (m_EnemyType == EnemyType::Recognizer) {
            m_TankController->TryMove(directionToTarget);
        }
        else {
            m_TankController->SetFacingDirection(directionToTarget);
        }

        return true;
    }

    return false;
}

bool EnemyTankAI::IsTargetVisible(dae::GameObject* target, Direction& directionToTarget) const {
    if (target == nullptr || m_TankController == nullptr) {
        return false;
    }

    uint16_t targetX{};
    uint16_t targetY{};
    if (!TryGetTargetTile(target, targetX, targetY)) {
        return false;
    }

    const uint16_t enemyX = m_TankController->GetCurrentTileX();
    const uint16_t enemyY = m_TankController->GetCurrentTileY();
    if (enemyX == targetX && enemyY == targetY) {
        return true;
    }

    if (enemyX == targetX) {
        directionToTarget = targetY < enemyY ? Direction::Up : Direction::Down;
        return HasClearLine(enemyX, enemyY, targetX, targetY);
    }

    if (enemyY == targetY) {
        directionToTarget = targetX < enemyX ? Direction::Left : Direction::Right;
        return HasClearLine(enemyX, enemyY, targetX, targetY);
    }

    return false;
}

bool EnemyTankAI::HasClearLine(uint16_t fromX, uint16_t fromY, uint16_t toX, uint16_t toY) const {
    if (m_LevelData == nullptr) {
        return false;
    }

    const int stepX = toX == fromX ? 0 : (toX > fromX ? 1 : -1);
    const int stepY = toY == fromY ? 0 : (toY > fromY ? 1 : -1);
    int x = int(fromX) + stepX;
    int y = int(fromY) + stepY;

    while (x != int(toX) || y != int(toY)) {
        if (x < 0 || y < 0 || m_LevelData->IsWall(uint16_t(x), uint16_t(y))) {
            return false;
        }

        x += stepX;
        y += stepY;
    }

    return true;
}

bool EnemyTankAI::TryGetTargetTile(dae::GameObject* target, uint16_t& tileX, uint16_t& tileY) const {
    if (target == nullptr || m_LevelData == nullptr || m_TileSize <= 0.0f) {
        return false;
    }

    const glm::vec3 position = target->transform.GetWorldPosition() - m_LevelOrigin;
    const int x = int(position.x / m_TileSize);
    const int y = int(position.y / m_TileSize);

    if (x < 0 || y < 0 || !m_LevelData->IsInside(uint16_t(x), uint16_t(y))) {
        return false;
    }

    tileX = uint16_t(x);
    tileY = uint16_t(y);
    return true;
}

std::vector<Direction> EnemyTankAI::GetPatrolOptions() const {
    std::vector<Direction> options{};
    if (m_TankController == nullptr) {
        return options;
    }

    const Direction forward = m_TankController->GetFacingDirection();
    const Direction left = TurnLeft(forward);
    const Direction right = TurnRight(forward);

    if (m_TankController->CanMove(forward)) {
        options.emplace_back(forward);
    }
    if (m_TankController->CanMove(left)) {
        options.emplace_back(left);
    }
    if (m_TankController->CanMove(right)) {
        options.emplace_back(right);
    }

    if (options.empty()) {
        options.emplace_back(Flip(forward));
    }

    return options;
}

Direction EnemyTankAI::TurnLeft(Direction direction) {
    switch (direction) {
        case Direction::Up:
            return Direction::Left;
        case Direction::Right:
            return Direction::Up;
        case Direction::Down:
            return Direction::Right;
        case Direction::Left:
            return Direction::Down;
    }

    return Direction::Up;
}

Direction EnemyTankAI::TurnRight(Direction direction) {
    switch (direction) {
        case Direction::Up:
            return Direction::Right;
        case Direction::Right:
            return Direction::Down;
        case Direction::Down:
            return Direction::Left;
        case Direction::Left:
            return Direction::Up;
    }

    return Direction::Up;
}

Direction EnemyTankAI::Flip(Direction direction) {
    switch (direction) {
        case Direction::Up:
            return Direction::Down;
        case Direction::Right:
            return Direction::Left;
        case Direction::Down:
            return Direction::Up;
        case Direction::Left:
            return Direction::Right;
    }

    return Direction::Up;
}
