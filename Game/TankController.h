//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_TANKCONTROLLER_H
#define PROG4MINIGINPROJECT_TANKCONTROLLER_H
#include "Component.h"
#include "LevelLoader.h"

#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class PlayerInputHandler;

class TankController : public dae::Component {
public:
    struct PlayerLevelData {
        const LevelData* levelData;
        glm::vec3 levelOrigin;
        float tileSize;
        uint16_t startTileX;
        uint16_t startTileY;
    };

    int GetFlags() override;
    void Update() override;

    void SetPlayerInput(const PlayerInputHandler* handler);
    void SetLevelData(const PlayerLevelData& playerData);
    void SetMoveSpeedTilesPerSecond(float speed);

    bool IsMoving() const;
    bool CanMove(Direction direction) const;
    bool TryMove(Direction direction);
    void SetFacingDirection(Direction direction);
    Direction GetFacingDirection() const;
    uint16_t GetCurrentTileX() const;
    uint16_t GetCurrentTileY() const;

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
    Direction m_FacingDirection{Direction::Up};

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
    static TilePosition DirectionToTilePosition(Direction direction);
};


#endif //PROG4MINIGINPROJECT_TANKCONTROLLER_H
