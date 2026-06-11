//
// Created by frane on 6/10/2026.
//

#ifndef PROG4MINIGINPROJECT_TELEPORTER_H
#define PROG4MINIGINPROJECT_TELEPORTER_H
#include "Component.h"

#include <glm/vec3.hpp>

struct LevelData;

namespace dae {
    class GameObject;
}

class Teleporter : public dae::Component {
public:
    int GetFlags() override;

    void Update() override;

    void Render() override;

    void SetLevelData(const LevelData* data);
    void SetLevelData(const LevelData* data, const glm::vec3& levelOrigin, float tileSize);

private:
    const LevelData* m_LevelData{};
    glm::vec3 m_LevelOrigin{};
    float m_TileSize{16.0f};
    float m_Cooldown{0.35f};
    float m_CooldownTimer{};

    glm::vec3 GetWorldPosition() const;
    bool IsPlayerOverlapping(dae::GameObject* player) const;
};


#endif //PROG4MINIGINPROJECT_TELEPORTER_H
