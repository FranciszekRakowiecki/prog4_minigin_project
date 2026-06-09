//
// Created by frane on 6/9/2026.
//

#include "LevelLoader.h"

#include "LevelLoader.h"

#include <stdexcept>
#include "ResourceManager.h"

bool LevelData::IsInside(uint16_t x, uint16_t y) const {
    return x < width && y < height;
}

bool LevelData::IsWall(uint16_t x, uint16_t y) const {
    if (!IsInside(x, y))
        return true;

    return tiles[y * width + x] == uint8_t(TileType::Wall);
}

LevelData LevelLoader::LoadFromFile(const std::string& file)
{
    const std::vector<uint8_t> bytes = dae::ResourceManager::GetInstance().LoadBinaryFile(file);
    return LoadFromBytes(bytes);
}

LevelData LevelLoader::LoadFromBytes(const std::vector<uint8_t>& bytes)
{
    size_t offset{};

    LevelData level{};
    level.width = ReadU16(bytes, offset);
    level.height = ReadU16(bytes, offset);

    const uint16_t spawnCount = ReadU16(bytes, offset);
    const uint16_t enemyCount = ReadU16(bytes, offset);

    const uint32_t tileCount = uint32_t(level.width) * uint32_t(level.height);
    if (offset + tileCount > bytes.size())
        throw std::runtime_error("Level tile data is incomplete");

    level.tiles.assign(bytes.begin() + offset, bytes.begin() + offset + tileCount);
    offset += tileCount;

    level.spawns.reserve(spawnCount);
    for (uint16_t i = 0; i < spawnCount; ++i)
    {
        LevelSpawnPoint spawn{};
        spawn.playerIndex = ReadU8(bytes, offset);
        spawn.tileX = ReadU16(bytes, offset);
        spawn.tileY = ReadU16(bytes, offset);
        spawn.direction = Direction(ReadU8(bytes, offset));
        level.spawns.emplace_back(std::move(spawn));
    }

    level.enemies.reserve(enemyCount);
    for (uint16_t i = 0; i < enemyCount; ++i)
    {
        LevelEnemySpawn enemy{};
        enemy.type = EnemyType(ReadU8(bytes, offset));
        enemy.tileX = ReadU16(bytes, offset);
        enemy.tileY = ReadU16(bytes, offset);
        enemy.direction = Direction(ReadU8(bytes, offset));
        level.enemies.emplace_back(std::move(enemy));
    }

    return level;
}

uint8_t LevelLoader::ReadU8(const std::vector<uint8_t>& bytes, size_t& offset)
{
    if (offset >= bytes.size())
        throw std::runtime_error("Unexpected end of level file");

    return bytes[offset++];
}

uint16_t LevelLoader::ReadU16(const std::vector<uint8_t>& bytes, size_t& offset)
{
    return ReadU8(bytes, offset) | (ReadU8(bytes, offset) << 8);
}