//
// Created by frane on 6/10/2026.
//

#include "Teleporter.h"

#include "Game.h"
#include "GameObject.h"
#include "GameTime.h"
#include "LevelLoader.h"
#include "PlayableGameState.h"
#include "Renderer.h"

int Teleporter::GetFlags() {
    return COMPONENT_HAS_RENDER | COMPONENT_HAS_UPDATE;
}

void Teleporter::Update() {
    if (m_LevelData == nullptr) {
        return;
    }

    if (m_CooldownTimer > 0.0f) {
        m_CooldownTimer -= dae::GameTime::GetInstance().GetDeltaTime();
        return;
    }

    PlayableGameState* playable = static_cast<PlayableGameState*>(Game::GetInstance().GetGameState());
    if (playable == nullptr) {
        return;
    }

    for (dae::GameObject* player : playable->GetPlayerObjects()) {
        if (player == nullptr || player->IsDestroyed() || !IsPlayerOverlapping(player)) {
            continue;
        }

        playable->RespawnPlayerAtRandomSpawn(player, false);
        m_CooldownTimer = m_Cooldown;
        return;
    }
}

void Teleporter::Render() {
    if (m_LevelData == nullptr) {
        return;
    }

    const glm::vec3 position = GetWorldPosition();
    dae::Renderer::GetInstance().RenderRect(position.x - m_TileSize / 2.0f, position.y - m_TileSize / 2.0f, m_TileSize, m_TileSize, {1.0f, 0.0f, 1.0f, 1.0f});
}

void Teleporter::SetLevelData(const LevelData *data) {
    m_LevelData = data;
}

void Teleporter::SetLevelData(const LevelData* data, const glm::vec3& levelOrigin, float tileSize) {
    m_LevelData = data;
    m_LevelOrigin = levelOrigin;
    m_TileSize = tileSize;
}

glm::vec3 Teleporter::GetWorldPosition() const {
    if (m_LevelData == nullptr) {
        return {};
    }

    return m_LevelOrigin + glm::vec3{
        float(m_LevelData->width / 2) * m_TileSize,
        float(m_LevelData->height / 2) * m_TileSize,
        0.0f
    };
}

bool Teleporter::IsPlayerOverlapping(dae::GameObject* player) const {
    if (player == nullptr) {
        return false;
    }

    const glm::vec3 teleporterPosition = GetWorldPosition();
    const glm::vec3 playerPosition = player->transform.GetWorldPosition();
    constexpr float halfPlayerSize = 10.0f;

    return playerPosition.x + halfPlayerSize >= teleporterPosition.x - m_TileSize / 2.0f
        && playerPosition.x - halfPlayerSize <= teleporterPosition.x + m_TileSize / 2.0f
        && playerPosition.y + halfPlayerSize >= teleporterPosition.y - m_TileSize / 2.0f
        && playerPosition.y - halfPlayerSize <= teleporterPosition.y + m_TileSize / 2.0f;
}
