//
// Created by frane on 6/10/2026.
//

#ifndef PROG4MINIGINPROJECT_PROJECTILESYSTEM_H
#define PROG4MINIGINPROJECT_PROJECTILESYSTEM_H

#include "Component.h"
#include "LevelLoader.h"

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace dae {
    class GameObject;
}

class ProjectileSystem : public dae::Component {
public:
    enum class ProjectileOwner {
        Player,
        Enemy
    };

    int GetFlags() override;
    void Update() override;
    void Render() override;

    void SetLevelData(const LevelData* levelData, const glm::vec3& levelOrigin, float tileSize);
    void SetPlayerProjectilesHitPlayers(bool enabled);
    bool GetPlayerProjectilesHitPlayers() const;

    void RegisterPlayer(dae::GameObject* player);
    void RegisterEnemy(dae::GameObject* enemy);
    void ClearTargets();

    void FirePlayerProjectile(dae::GameObject* owner, const glm::vec3& startPosition, Direction direction);
    void FireEnemyProjectile(dae::GameObject* owner, const glm::vec3& startPosition, Direction direction);
    void FireProjectile(ProjectileOwner ownerType, dae::GameObject* owner, const glm::vec3& startPosition, Direction direction);

private:
    struct Projectile {
        bool active{false};
        ProjectileOwner ownerType{ProjectileOwner::Player};
        dae::GameObject* owner{nullptr};
        glm::vec2 position{};
        glm::vec2 direction{};
        uint32_t bouncesRemaining{5};
    };

    const LevelData* m_LevelData{nullptr};
    glm::vec3 m_LevelOrigin{};
    float m_TileSize{16.0f};
    float m_ProjectileSpeed{240.0f};
    float m_ProjectileSize{6.0f};
    int m_MaxBounces{5};
    bool m_PlayerProjectilesHitPlayers{false};

    std::vector<Projectile> m_Projectiles{};
    std::vector<dae::GameObject*> m_PlayerTargets{};
    std::vector<dae::GameObject*> m_EnemyTargets{};

    Projectile& CreateProjectile();
    void UpdateProjectile(Projectile& projectile, float deltaTime);
    bool TryBounceProjectile(Projectile& projectile, const glm::vec2& nextPosition);
    bool IsWallAtPosition(const glm::vec2& position) const;
    bool TryHitTarget(Projectile& projectile);
    bool TryHitAnyTarget(Projectile& projectile, std::vector<dae::GameObject*>& targets);
    bool HitsGameObject(const Projectile& projectile, dae::GameObject* target) const;
    void HandleTargetHit(Projectile& projectile, dae::GameObject* target);

    static glm::vec2 DirectionToVector(Direction direction);
};

#endif //PROG4MINIGINPROJECT_PROJECTILESYSTEM_H
