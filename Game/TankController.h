//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_TANKCONTROLLER_H
#define PROG4MINIGINPROJECT_TANKCONTROLLER_H
#include "Component.h"

#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class PlayerInputHandler;
struct LevelData;

class TankController : public dae::Component {
public:
    struct PlayerLevelData {
        const LevelData* levelData;
        const glm::vec3& levelOrigin;
        float tileSize;
        uint16_t startTileX;
        uint16_t startTileY;
    };

    int GetFlags() override;
    void Update() override;

    void SetPlayerInput(const PlayerInputHandler* handler);
    void SetLevelData(const PlayerLevelData& playerData);

private:
    const PlayerInputHandler* m_PlayerInput{nullptr};
    const LevelData* m_LevelData{nullptr};

    struct TilePosition {
        int x{};
        int y{};
    };

    glm::vec3 m_LevelOrigin{};
    float m_TileSize{16.0f};
    float m_MoveSpeedTilesPerSecond{6.0f};

    TilePosition m_CurrentTile{};
    TilePosition m_TargetTile{};
    glm::vec3 m_MoveStartPosition{};
    glm::vec3 m_TargetPosition{};
    float m_MoveProgress{1.0f};

    glm::vec3 TileCenterToWorld(const TilePosition& tile) const;
    bool CanMoveTo(const TilePosition& tile) const;
    TilePosition GetRequestedDirection() const;
    void StartMove(const TilePosition& direction);
    void SetFacingDirection(const TilePosition& direction);
};


#endif //PROG4MINIGINPROJECT_TANKCONTROLLER_H
