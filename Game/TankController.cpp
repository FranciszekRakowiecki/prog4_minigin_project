//
// Created by frane on 6/9/2026.
//

#include "TankController.h"

#include <algorithm>
#include <cmath>

#include "GameObject.h"
#include "GameTime.h"
#include "LevelLoader.h"
#include "PlayerInputHandler.h"

int TankController::GetFlags() {
    return COMPONENT_HAS_UPDATE;
}

void TankController::Update() {
    if (m_LevelData == nullptr || m_PlayerInput == nullptr) {
        return;
    }

    if (m_MoveProgress < 1.0f) {
        const float moveStep = dae::GameTime::GetInstance().GetDeltaTime() * m_MoveSpeedTilesPerSecond;
        m_MoveProgress = std::min(1.0f, m_MoveProgress + moveStep);

        const glm::vec3 position = m_MoveStartPosition + (m_TargetPosition - m_MoveStartPosition) * m_MoveProgress;
        GetParent()->transform.SetWorldPosition(position);

        if (m_MoveProgress < 1.0f) {
            return;
        }

        m_CurrentTile = m_TargetTile;
        GetParent()->transform.SetWorldPosition(m_TargetPosition);
    }

    const TilePosition direction = GetRequestedDirection();
    if (direction.x != 0 || direction.y != 0) {
        SetFacingDirection(direction);
        StartMove(direction);
    }
}

void TankController::SetPlayerInput(const PlayerInputHandler *handler) {
    m_PlayerInput = handler;
}

void TankController::SetLevelData(const PlayerLevelData &playerData) {
    m_LevelData = playerData.levelData;
    m_LevelOrigin = playerData.levelOrigin;
    m_TileSize = playerData.tileSize;
    m_CurrentTile = {playerData.startTileX, playerData.startTileY};
    m_TargetTile = m_CurrentTile;
    m_MoveProgress = 1.0f;
    m_TargetPosition = TileCenterToWorld(m_CurrentTile);

    GetParent()->transform.SetWorldPosition(m_TargetPosition);
}

glm::vec3 TankController::TileCenterToWorld(const TilePosition& tile) const {
    return m_LevelOrigin + glm::vec3{
        (float(tile.x) + 0.5f) * m_TileSize,
        (float(tile.y) + 0.5f) * m_TileSize,
        0.0f
    };
}

bool TankController::CanMoveTo(const TilePosition& tile) const {
    if (m_LevelData == nullptr || tile.x < 0 || tile.y < 0) {
        return false;
    }

    return !m_LevelData->IsWall(uint16_t(tile.x), uint16_t(tile.y));
}

TankController::TilePosition TankController::GetRequestedDirection() const {
    if (m_PlayerInput == nullptr) {
        return {};
    }

    const glm::vec2 move = m_PlayerInput->GetMove();
    if (std::abs(move.x) > std::abs(move.y)) {
        if (move.x > 0.0f) {
            return {1, 0};
        }
        if (move.x < 0.0f) {
            return {-1, 0};
        }
    }
    else {
        if (move.y > 0.0f) {
            return {0, -1};
        }
        if (move.y < 0.0f) {
            return {0, 1};
        }
    }

    return {};
}

void TankController::StartMove(const TilePosition& direction) {
    const TilePosition nextTile{m_CurrentTile.x + direction.x, m_CurrentTile.y + direction.y};
    if (!CanMoveTo(nextTile)) {
        return;
    }

    m_TargetTile = nextTile;
    m_MoveStartPosition = TileCenterToWorld(m_CurrentTile);
    m_TargetPosition = TileCenterToWorld(m_TargetTile);
    m_MoveProgress = 0.0f;
}

void TankController::SetFacingDirection(const TilePosition& direction) {
    constexpr float halfPi = 1.57079632679f;
    constexpr float pi = 3.14159265359f;

    if (direction.x > 0) {
        GetParent()->transform.SetRotation(0.0f);
    }
    else if (direction.x < 0) {
        GetParent()->transform.SetRotation(pi);
    }
    else if (direction.y > 0) {
        GetParent()->transform.SetRotation(halfPi);
    }
    else if (direction.y < 0) {
        GetParent()->transform.SetRotation(-halfPi);
    }
}
