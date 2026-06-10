//
// Created by frane on 6/10/2026.
//

#include "ProjectileSystem.h"

#include <algorithm>
#include <cmath>

#include "GameObject.h"
#include "GameTime.h"
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

void ProjectileSystem::RegisterPlayer(dae::GameObject* player) {
    if (player == nullptr) {
        return;
    }

    if (std::find(m_PlayerTargets.begin(), m_PlayerTargets.end(), player) == m_PlayerTargets.end()) {
        m_PlayerTargets.emplace_back(player);
    }
}

void ProjectileSystem::RegisterEnemy(dae::GameObject* enemy) {
    if (enemy == nullptr) {
        return;
    }

    if (std::find(m_EnemyTargets.begin(), m_EnemyTargets.end(), enemy) == m_EnemyTargets.end()) {
        m_EnemyTargets.emplace_back(enemy);
    }
}

void ProjectileSystem::ClearTargets() {
    m_PlayerTargets.clear();
    m_EnemyTargets.clear();
}

void ProjectileSystem::FirePlayerProjectile(dae::GameObject* owner, const glm::vec3& startPosition, Direction direction) {
    FireProjectile(ProjectileOwner::Player, owner, startPosition, direction);
}

void ProjectileSystem::FireEnemyProjectile(dae::GameObject* owner, const glm::vec3& startPosition, Direction direction) {
    FireProjectile(ProjectileOwner::Enemy, owner, startPosition, direction);
}

void ProjectileSystem::FireProjectile(ProjectileOwner ownerType, dae::GameObject* owner, const glm::vec3& startPosition, Direction direction) {
    Projectile& projectile = CreateProjectile();
    projectile.active = true;
    projectile.ownerType = ownerType;
    projectile.owner = owner;
    projectile.position = {startPosition.x, startPosition.y};
    projectile.direction = DirectionToVector(direction);
    projectile.bouncesRemaining = m_MaxBounces;
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
    if (projectile.ownerType == ProjectileOwner::Enemy) {
        return TryHitAnyTarget(projectile, m_PlayerTargets);
    }

    if (TryHitAnyTarget(projectile, m_EnemyTargets)) {
        return true;
    }

    if (m_PlayerProjectilesHitPlayers) {
        return TryHitAnyTarget(projectile, m_PlayerTargets);
    }

    return false;
}

bool ProjectileSystem::TryHitAnyTarget(Projectile& projectile, std::vector<dae::GameObject*>& targets) {
    for (int index = int(targets.size()) - 1; index >= 0; --index) {
        dae::GameObject* target = targets[index];
        if (target == nullptr || target->IsDestroyed()) {
            targets.erase(targets.begin() + index);
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
        --playerHealth->lives;
        return;
    }

    target->Destroy();
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
