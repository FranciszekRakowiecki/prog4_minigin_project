//
// Created by frane on 6/9/2026.
//

#include "PlayableGameState.h"

#include <cstddef>
#include <iostream>
#include <memory>

#include "Game.h"
#include "GameObject.h"
#include "LevelLoader.h"
#include "LevelRenderer.h"
#include "Minigin.h"
#include "PlayerHealth.h"
#include "Scene.h"
#include "SceneManager.h"
#include "TankController.h"
#include "TankRenderer.h"
#include "glm/vec2.hpp"

void PlayableGameState::Enter() {
    LoadLevel0();
}

void PlayableGameState::Exit() {
    UnloadCurrentScene();
}

void PlayableGameState::OnLevelSkip() {
    LoadLevel((m_CurrentLevelIndex + 1) % 3);
}

void PlayableGameState::LoadLevel(uint32_t index) {
    switch (index) {
        case 0:
            LoadLevel0();
            break;
        case 1:
            LoadLevel1();
            break;
        case 2:
            LoadLevel2();
            break;
        default:
            LoadLevel0();
            break;
    }
}

void PlayableGameState::LoadLevel0() {
    LoadLevelWithData(Game::GetInstance().GetLevel0(), 0);
}

void PlayableGameState::LoadLevel1() {
    LoadLevelWithData(Game::GetInstance().GetLevel1(), 1);
}

void PlayableGameState::LoadLevel2() {
    LoadLevelWithData(Game::GetInstance().GetLevel2(), 2);
}

void PlayableGameState::LoadLevelWithData(LevelData* data, uint32_t index) {
    UnloadCurrentScene();

    m_Scene = dae::SceneManager::GetInstance().CreateScene();
    m_CurrentLevel = data;
    m_CurrentLevelIndex = index;

    if (m_Scene == nullptr || m_CurrentLevel == nullptr) {
        return;
    }

    std::unique_ptr<dae::GameObject> levelObject{std::make_unique<dae::GameObject>()};
    dae::Reference<LevelRenderer> renderer{levelObject->AddComponent<LevelRenderer>()};

    renderer->SetLevelData(m_CurrentLevel);
    levelObject->transform.SetWorldPosition(TileToWorld(0, 0));

    m_Scene->Add(std::move(levelObject));

    OnLevelLoad(m_CurrentLevel, m_CurrentLevelIndex);
}

dae::GameObject* PlayableGameState::SpawnPlayer(const PlayerInputHandler* handler) {
    if (m_Scene == nullptr || m_CurrentLevel == nullptr || handler == nullptr) {
        return nullptr;
    }

    const LevelSpawnPoint* spawnPoint = FindSpawnPoint(handler->GetPlayerIndex());
    if (spawnPoint == nullptr) {
        std::cout << "No spawn point found for player " << handler->GetPlayerIndex() << "." << std::endl;
        return nullptr;
    }

    std::unique_ptr<dae::GameObject> playerObject = std::make_unique<dae::GameObject>();
    dae::GameObject* player = playerObject.get();

    playerObject->AddComponent<TankRenderer>();
    dae::Reference<TankController> tankController = playerObject->AddComponent<TankController>();

    playerObject->AddComponent<PlayerHealth>();
    tankController->SetPlayerInput(handler);

    const glm::vec3 levelOrigin = TileToWorld(0, 0);
    const float tileSize = TileToWorld(1, 0).x - levelOrigin.x;
    TankController::PlayerLevelData data {
    m_CurrentLevel,
        levelOrigin,
        tileSize,
        spawnPoint->tileX,
        spawnPoint->tileY
    };
    tankController->SetLevelData(data);

    m_Scene->Add(std::move(playerObject));
    return player;
}

void PlayableGameState::SpawnEnemies() {
    if (m_Scene == nullptr || m_CurrentLevel == nullptr) {
        return;
    }
}

void PlayableGameState::RespawnPlayer(dae::GameObject* player) {
    if (player == nullptr || m_CurrentLevel == nullptr) {
        return;
    }

    dae::Reference<PlayerHealth> health = player->GetComponent<PlayerHealth>();
    if (health) {
        --health->lives;
        std::cout << "Player has lost a life and respawned." << std::endl;
    }

    const LevelSpawnPoint& spawn = m_CurrentLevel->spawns[0];
    const glm::vec3 levelOrigin = TileToWorld(0, 0);
    const float tileSize = TileToWorld(1, 0).x - levelOrigin.x;

    dae::Reference<TankController> tankController = player->GetComponent<TankController>();
    if (tankController) {
        TankController::PlayerLevelData data {
            m_CurrentLevel,
            levelOrigin,
            tileSize,
            spawn.tileX,
            spawn.tileY
        };
        tankController->SetLevelData(data);
        return;
    }

    player->transform.SetWorldPosition(TileToWorld(spawn.tileX, spawn.tileY) + glm::vec3{tileSize * 0.5f, tileSize * 0.5f, 0.0f});
}

glm::vec3 PlayableGameState::TileToWorld(uint32_t x, uint32_t y) const {
    if (m_CurrentLevel == nullptr || m_CurrentLevel->height == 0) {
        return glm::vec3{};
    }

    const glm::vec2 windowSize{dae::Minigin::GetInstance().GetWindowSize()};
    const float tileSize = (windowSize.y - 100.0f) / float(m_CurrentLevel->height);
    const float levelResolution{m_CurrentLevel->height * tileSize};
    const glm::vec3 levelOffset{windowSize.x / 2.0f - levelResolution / 2.0f, 100.0f, 0.0f};

    return levelOffset + glm::vec3{x * tileSize, y * tileSize, 0.0f};
}

dae::Scene* PlayableGameState::GetScene() const {
    return m_Scene;
}

void PlayableGameState::UnloadCurrentScene() {
    if (m_Scene != nullptr) {
        dae::SceneManager::GetInstance().UnloadScene(m_Scene);
        m_Scene = nullptr;
    }

    m_CurrentLevel = nullptr;
}

const LevelSpawnPoint* PlayableGameState::FindSpawnPoint(uint32_t playerIndex) const {
    if (m_CurrentLevel == nullptr) {
        return nullptr;
    }

    for (size_t index = 0; index < m_CurrentLevel->spawns.size(); ++index) {
        const LevelSpawnPoint* spawnPoint = &m_CurrentLevel->spawns[index];
        if (spawnPoint->playerIndex == playerIndex) {
            return spawnPoint;
        }
    }

    return nullptr;
}

LevelData* PlayableGameState::GetCurrentLevelData() const {
    return m_CurrentLevel;
}

uint32_t PlayableGameState::GetCurrentLevelIndex() const {
    return m_CurrentLevelIndex;
}
