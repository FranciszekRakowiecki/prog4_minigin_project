//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_LEVELLOADER_H
#define PROG4MINIGINPROJECT_LEVELLOADER_H

#include <cstdint>
#include <string>
#include <vector>

enum class TileType : uint8_t
{
    Empty = 0,
    Wall = 1
};

enum class Direction : uint8_t
{
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3
};

enum class EnemyType : uint8_t
{
    BlueTank = 0,
    Recognizer = 1
};

struct LevelSpawnPoint
{
    uint8_t playerIndex{};
    uint16_t tileX{};
    uint16_t tileY{};
    Direction direction{Direction::Up};
};

struct LevelEnemySpawn
{
    EnemyType type{EnemyType::BlueTank};
    uint16_t tileX{};
    uint16_t tileY{};
    Direction direction{Direction::Up};
};

struct LevelData
{
    uint16_t width{};
    uint16_t height{};
    std::vector<uint8_t> tiles;
    std::vector<LevelSpawnPoint> spawns;
    std::vector<LevelEnemySpawn> enemies;

    bool IsInside(uint16_t x, uint16_t y) const;

    bool IsWall(uint16_t x, uint16_t y) const;
};

class LevelLoader {
public:
    static LevelData LoadFromFile(const std::string& file);
    static LevelData LoadFromBytes(const std::vector<uint8_t>& bytes);

private:
    static uint8_t ReadU8(const std::vector<uint8_t> &bytes, size_t &offset);

    static uint16_t ReadU16(const std::vector<uint8_t> &bytes, size_t &offset);
};


#endif //PROG4MINIGINPROJECT_LEVELLOADER_H