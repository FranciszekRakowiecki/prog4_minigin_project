//
// Created by frane on 6/10/2026.
//

#include "ProjectileSystem.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

#include "EnemyTankAI.h"
#include "Game.h"
#include "GameObject.h"
#include "GameTime.h"
#include "PlayableGameState.h"
#include "PlayerHealth.h"
#include "Renderer.h"

int ProjectileSystem::GetFlags() {
    return COMPONENT_HAS_UPDATE | COMPONENT_HAS_RENDER;
}

void ProjectileSystem::Update() {
    const float deltaTime = dae::GameTime::GetInstance().GetDeltaTime();
    for (Projectile& projectile : m_Projectiles) {
        if (!projectile.active) {
            continue;
        }

        UpdateProjectile(projectile, deltaTime);
    }
}

void ProjectileSystem::Render() {
    for (const Projectile& projectile : m_Projectiles) {
        if (!projectile.active) {
            continue;
        }

        const glm::vec4 color = projectile.ownerType == ProjectileOwner::Player ? glm::vec4{0.2f, 1.0f, 0.35f, 1.0f}: glm::vec4{1.0f, 0.2f, 0.2f, 1.0f};

        dae::Renderer::GetInstance().RenderRect(projectile.position.x - m_ProjectileSize * 0.5f,projectile.position.y - m_ProjectileSize * 0.5f,m_ProjectileSize,m_ProjectileSize,color);
    }
}

void ProjectileSystem::SetLevelData(const LevelData* levelData, const glm::vec3& levelOrigin, float tileSize) {
    m_LevelData = levelData;
    m_LevelOrigin = levelOrigin;
    m_TileSize = tileSize;
    m_Projectiles.clear();
}

void ProjectileSystem::SetPlayerProjectilesHitPlayers(bool enabled) {
    m_PlayerProjectilesHitPlayers = enabled;
}

bool ProjectileSystem::GetPlayerProjectilesHitPlayers() const {
    return m_PlayerProjectilesHitPlayers;
}

void ProjectileSystem::FirePlayerProjectile(dae::GameObject* owner, const glm::vec3& startPosition, glm::vec2 direction) {
    FireProjectile(ProjectileOwner::Player, owner, startPosition, direction, m_MaxBounces);
    Game::GetInstance().PlaySFX(GameSFX::PLAYER_SHOOT);
}

void ProjectileSystem::FireEnemyProjectile(dae::GameObject* owner, const glm::vec3& startPosition, Direction direction) {
    FireProjectile(ProjectileOwner::Enemy, owner, startPosition, DirectionToVector(direction), 0);
    Game::GetInstance().PlaySFX(GameSFX::ENEMY_SHOOT);
}

void ProjectileSystem::FireProjectile(ProjectileOwner ownerType, dae::GameObject* owner, const glm::vec3& startPosition, glm::vec2 direction, uint32_t bouncesRemaining) {
    Projectile& projectile = CreateProjectile();
    projectile.active = true;
    projectile.ownerType = ownerType;
    projectile.owner = owner;
    projectile.position = {startPosition.x, startPosition.y};
    projectile.direction = glm::normalize(direction);
    projectile.bouncesRemaining = bouncesRemaining;
}

ProjectileSystem::Projectile& ProjectileSystem::CreateProjectile() {
    for (Projectile& projectile : m_Projectiles) {
        if (!projectile.active) {
            return projectile;
        }
    }

    return m_Projectiles.emplace_back();
}

void ProjectileSystem::UpdateProjectile(Projectile& projectile, float deltaTime) {
    if (TryHitTarget(projectile)) {
        return;
    }

    const glm::vec2 nextPosition = projectile.position + projectile.direction * m_ProjectileSpeed * deltaTime;
    if (!TryBounceProjectile(projectile, nextPosition)) {
        projectile.position = nextPosition;
    }

    TryHitTarget(projectile);
}

bool ProjectileSystem::TryBounceProjectile(Projectile& projectile, const glm::vec2& nextPosition) {
    if (!IsWallAtPosition(nextPosition)) {
        return false;
    }

    if (projectile.bouncesRemaining <= 0) {
        projectile.active = false;
        return true;
    }

    const bool wallOnX = IsWallAtPosition({nextPosition.x, projectile.position.y});
    const bool wallOnY = IsWallAtPosition({projectile.position.x, nextPosition.y});

    if (wallOnX) {
        projectile.direction.x *= -1.0f;
    }
    if (wallOnY) {
        projectile.direction.y *= -1.0f;
    }
    if (!wallOnX && !wallOnY) {
        projectile.direction *= -1.0f;
    }

    --projectile.bouncesRemaining;
    return true;
}

bool ProjectileSystem::IsWallAtPosition(const glm::vec2& position) const {
    if (m_LevelData == nullptr || m_TileSize <= 0.0f) {
        return true;
    }

    const glm::vec2 localPosition{position.x - m_LevelOrigin.x, position.y - m_LevelOrigin.y};
    const int tileX = int(std::floor(localPosition.x / m_TileSize));
    const int tileY = int(std::floor(localPosition.y / m_TileSize));

    if (tileX < 0 || tileY < 0) {
        return true;
    }

    return m_LevelData->IsWall(uint16_t(tileX), uint16_t(tileY));
}

bool ProjectileSystem::TryHitTarget(Projectile& projectile) {
    PlayableGameState* playable = GetPlayableGameState();
    if (playable == nullptr) {
        return false;
    }

    if (projectile.ownerType == ProjectileOwner::Enemy) {
        return TryHitAnyTarget(projectile, playable->GetPlayerObjects());
    }

    if (TryHitAnyTarget(projectile, playable->GetEnemyObjects())) {
        return true;
    }

    if (m_PlayerProjectilesHitPlayers) {
        return TryHitAnyTarget(projectile, playable->GetPlayerObjects());
    }

    return false;
}

bool ProjectileSystem::TryHitAnyTarget(Projectile& projectile, const std::vector<dae::GameObject*>& targets) {
    for (int index = int(targets.size()) - 1; index >= 0; --index) {
        dae::GameObject* target = targets[index];
        if (target == nullptr || target->IsDestroyed()) {
            continue;
        }

        if (target == projectile.owner || !HitsGameObject(projectile, target)) {
            continue;
        }

        HandleTargetHit(projectile, target);
        return true;
    }

    return false;
}

bool ProjectileSystem::HitsGameObject(const Projectile& projectile, dae::GameObject* target) const {
    if (target == nullptr) {
        return false;
    }

    const glm::vec3 targetPosition = target->transform.GetWorldPosition();
    constexpr float halfTankSize = 10.0f;
    const float halfProjectileSize = m_ProjectileSize * 0.5f;

    return projectile.position.x + halfProjectileSize >= targetPosition.x - halfTankSize
        && projectile.position.x - halfProjectileSize <= targetPosition.x + halfTankSize
        && projectile.position.y + halfProjectileSize >= targetPosition.y - halfTankSize
        && projectile.position.y - halfProjectileSize <= targetPosition.y + halfTankSize;
}

void ProjectileSystem::HandleTargetHit(Projectile& projectile, dae::GameObject* target) {
    projectile.active = false;

    dae::Reference<PlayerHealth> playerHealth = target->GetComponent<PlayerHealth>();
    if (playerHealth) {
        Game::GetInstance().PlaySFX(GameSFX::PLAYER_DEATH);
        PlayableGameState* playable = GetPlayableGameState();
        if (playable != nullptr) {
            playable->RespawnPlayerAtRandomSpawn(target, true);
        }
        return;
    }

    dae::Reference<EnemyTankAI> enemyAI = target->GetComponent<EnemyTankAI>();
    if (enemyAI) {
        const uint32_t scoreValue = enemyAI->GetScoreValue();
        enemyAI->TakeDamage();
        if (target->IsDestroyed() && projectile.ownerType == ProjectileOwner::Player) {
            AwardPlayerScore(projectile.owner, scoreValue);
            Game::GetInstance().PlaySFX(GameSFX::ENEMY_DEATH);
        }
        return;
    }

    target->Destroy();
}

void ProjectileSystem::AwardPlayerScore(dae::GameObject* player, uint32_t score) {
    if (player == nullptr || score == 0) {
        return;
    }

    PlayableGameState* playable = GetPlayableGameState();
    if (playable == nullptr) {
        return;
    }

    const std::vector<dae::GameObject*>& players = playable->GetPlayerObjects();
    for (uint32_t index = 0; index < players.size(); ++index) {
        if (players[index] == player) {
            Game::GetInstance().GetScoreKeeper().AddPlayerScore(index, score);
            return;
        }
    }
}

glm::vec2 ProjectileSystem::DirectionToVector(Direction direction) {
    switch (direction) {
        case Direction::Up:
            return {0.0f, -1.0f};
        case Direction::Right:
            return {1.0f, 0.0f};
        case Direction::Down:
            return {0.0f, 1.0f};
        case Direction::Left:
            return {-1.0f, 0.0f};
    }

    return {};
}

PlayableGameState* ProjectileSystem::GetPlayableGameState() const {
    return static_cast<PlayableGameState*>(Game::GetInstance().GetGameState());
}
