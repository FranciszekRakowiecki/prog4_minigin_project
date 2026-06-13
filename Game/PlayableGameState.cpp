//
// Created by frane on 6/9/2026.
//

#include "PlayableGameState.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <random>

#include "Game.h"
#include "EnemyTankAI.h"
#include "GameObject.h"
#include "LevelLoader.h"
#include "LevelRenderer.h"
#include "Minigin.h"
#include "PlayerHealth.h"
#include "PlayerHealthDisplay.h"
#include "PlayerScoreDisplay.h"
#include "ProjectileSystem.h"
#include "Scene.h"
#include "SceneManager.h"
#include "TankController.h"
#include "TankRenderer.h"
#include "Teleporter.h"
#include "TextRenderer.h"
#include "glm/vec2.hpp"

void PlayableGameState::Enter() {
    LoadLevel0();
}

void PlayableGameState::Exit() {
    UnloadCurrentScene();
}

void PlayableGameState::Update() {
    CheckLevelCompletion();
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

    Game::GetInstance().PlaySFX(GameSFX::ENTER_LEVEL);

    m_Scene = dae::SceneManager::GetInstance().CreateScene();
    m_CurrentLevel = data;
    m_CurrentLevelIndex = index;
    m_IsCompletingLevel = false;
    m_CompleteLevelWhenEnemiesDestroyed = true;
    m_EndGameWhenOnePlayerRemains = false;

    if (m_Scene == nullptr || m_CurrentLevel == nullptr) {
        return;
    }

    std::unique_ptr<dae::GameObject> levelObject{std::make_unique<dae::GameObject>()};
    dae::Reference<LevelRenderer> renderer{levelObject->AddComponent<LevelRenderer>()};

    renderer->SetLevelData(m_CurrentLevel);
    levelObject->transform.SetWorldPosition(TileToWorld(0, 0));

    m_Scene->Add(std::move(levelObject));

    const glm::vec3 levelOrigin = TileToWorld(0, 0);
    const float tileSize = TileToWorld(1, 0).x - levelOrigin.x;
    std::unique_ptr<dae::GameObject> projectileObject{std::make_unique<dae::GameObject>()};
    dae::Reference<ProjectileSystem> projectileSystem = projectileObject->AddComponent<ProjectileSystem>();

    projectileSystem->SetLevelData(m_CurrentLevel, levelOrigin, tileSize);
    m_ProjectileSystem = projectileSystem.get();

    m_Scene->Add(std::move(projectileObject));

    std::unique_ptr<dae::GameObject> teleporterObject{std::make_unique<dae::GameObject>()};
    dae::Reference<Teleporter> teleporter = teleporterObject->AddComponent<Teleporter>();
    teleporter->SetLevelData(m_CurrentLevel, levelOrigin, tileSize);
    m_Scene->Add(std::move(teleporterObject));

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

    dae::Reference<PlayerHealth> healthComponent = playerObject->AddComponent<PlayerHealth>();
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
    tankController->SetFacingDirection(spawnPoint->direction);

    m_Scene->Add(std::move(playerObject));
    m_PlayerObjects.emplace_back(player);

    {
        std::unique_ptr<dae::GameObject> gameobject = std::make_unique<dae::GameObject>();
        dae::Reference<TextRenderer> text = gameobject->AddComponent<TextRenderer>();
        dae::Reference<PlayerHealthDisplay> healthDisplay = gameobject->AddComponent<PlayerHealthDisplay>();
        healthDisplay->SetPlayerHealth(healthComponent);
        if (handler->GetPlayerIndex() == 0)
            healthDisplay->SetPrefix("P1 Lives: ");
        else {
            healthDisplay->SetPrefix("P2 Lives: ");
            gameobject->transform.SetWorldPosition(0.0f, 50.0f);
        }

        m_Scene->Add(std::move(gameobject));
    }

    {
        std::unique_ptr<dae::GameObject> gameobject = std::make_unique<dae::GameObject>();
        gameobject->AddComponent<TextRenderer>();
        dae::Reference<PlayerScoreDisplay> scoreDisplay = gameobject->AddComponent<PlayerScoreDisplay>();
        scoreDisplay->SetPlayerIndex(handler->GetPlayerIndex());
        if (handler->GetPlayerIndex() == 0) {
            scoreDisplay->SetPrefix("P1 Score: ");
            gameobject->transform.SetWorldPosition(0.0f, 25.0f);
        }
        else {
            scoreDisplay->SetPrefix("P2 Score: ");
            gameobject->transform.SetWorldPosition(0.0f, 75.0f);
        }

        m_Scene->Add(std::move(gameobject));
    }

    return player;
}

void PlayableGameState::SpawnEnemies() {
    if (m_Scene == nullptr || m_CurrentLevel == nullptr) {
        return;
    }

    const glm::vec3 levelOrigin = TileToWorld(0, 0);
    const float tileSize = TileToWorld(1, 0).x - levelOrigin.x;

    for (const LevelEnemySpawn& enemySpawn : m_CurrentLevel->enemies) {
        std::unique_ptr<dae::GameObject> enemyObject = std::make_unique<dae::GameObject>();
        dae::GameObject* enemy = enemyObject.get();

        dae::Reference<TankRenderer> tankRenderer = enemyObject->AddComponent<TankRenderer>();
        dae::Reference<TankController> tankController = enemyObject->AddComponent<TankController>();
        dae::Reference<EnemyTankAI> enemyAI = enemyObject->AddComponent<EnemyTankAI>();

        if (enemySpawn.type == EnemyType::Recognizer) {
            tankRenderer->SetTankColor({1.0f, 0.85f, 0.1f, 1.0f});
            tankController->SetMoveSpeedTilesPerSecond(6.0f);
        }
        else {
            tankRenderer->SetTankColor({0.1f, 0.35f, 1.0f, 1.0f});
            tankController->SetMoveSpeedTilesPerSecond(4.0f);
        }

        TankController::PlayerLevelData data {
            m_CurrentLevel,
            levelOrigin,
            tileSize,
            enemySpawn.tileX,
            enemySpawn.tileY
        };
        tankController->SetLevelData(data);
        tankController->SetFacingDirection(enemySpawn.direction);

        enemyAI->SetEnemyType(enemySpawn.type);
        enemyAI->SetLevelData(m_CurrentLevel, levelOrigin, tileSize);

        m_Scene->Add(std::move(enemyObject));
        m_EnemyObjects.emplace_back(enemy);
    }
}

void PlayableGameState::RespawnPlayer(dae::GameObject* player) {
    RespawnPlayerAtRandomSpawn(player, true);
}

void PlayableGameState::SetCompleteLevelWhenEnemiesDestroyed(bool enabled) {
    m_CompleteLevelWhenEnemiesDestroyed = enabled;
}

void PlayableGameState::SetEndGameWhenOnePlayerRemains(bool enabled) {
    m_EndGameWhenOnePlayerRemains = enabled;
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

ProjectileSystem* PlayableGameState::GetProjectileSystem() const {
    return m_ProjectileSystem;
}

void PlayableGameState::RespawnPlayerAtRandomSpawn(dae::GameObject* player, bool loseLife) {
    if (player == nullptr || m_CurrentLevel == nullptr || m_CurrentLevel->spawns.empty()) {
        return;
    }

    dae::Reference<PlayerHealth> health = player->GetComponent<PlayerHealth>();
    if (loseLife && health) {
        --health->lives;
        if (health->lives <= 0) {
            player->Destroy();
            return;
        }
    }

    std::uniform_int_distribution<std::size_t> distribution{0, m_CurrentLevel->spawns.size() - 1};
    const LevelSpawnPoint& spawn = m_CurrentLevel->spawns[distribution(m_RandomGenerator)];
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
        tankController->SetFacingDirection(spawn.direction);
        return;
    }

    player->transform.SetWorldPosition(TileToWorld(spawn.tileX, spawn.tileY) + glm::vec3{tileSize * 0.5f, tileSize * 0.5f, 0.0f});
}

void PlayableGameState::UnloadCurrentScene() {
    m_PlayerObjects.clear();
    m_EnemyObjects.clear();
    m_ProjectileSystem = nullptr;
    m_IsCompletingLevel = false;

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

const std::vector<dae::GameObject*>& PlayableGameState::GetPlayerObjects() const {
    return m_PlayerObjects;
}

const std::vector<dae::GameObject*>& PlayableGameState::GetEnemyObjects() const {
    return m_EnemyObjects;
}

void PlayableGameState::CheckLevelCompletion() {
    if (m_IsCompletingLevel || m_CurrentLevel == nullptr) {
        return;
    }

    RemoveDestroyedObjects(m_PlayerObjects);
    RemoveDestroyedObjects(m_EnemyObjects);

    if (AreAllPlayersDead()) {
        EndGame();
        return;
    }

    if (m_EndGameWhenOnePlayerRemains && IsOnePlayerRemaining()) {
        EndGame();
        return;
    }

    if (m_CompleteLevelWhenEnemiesDestroyed && AreAllEnemiesDestroyed()) {
        CompleteLevel();
    }
}

void PlayableGameState::RemoveDestroyedObjects(std::vector<dae::GameObject*>& objects) {
    std::erase_if(objects, [](dae::GameObject* object) {
        return object == nullptr || object->IsDestroyed();
    });
}

bool PlayableGameState::AreAllPlayersDead() const {
    if (m_PlayerObjects.empty()) {
        return true;
    }

    for (dae::GameObject* player : m_PlayerObjects) {
        if (player == nullptr || player->IsDestroyed()) {
            continue;
        }

        dae::Reference<PlayerHealth> health = player->GetComponent<PlayerHealth>();
        if (!health || health->lives > 0) {
            return false;
        }
    }

    return true;
}

bool PlayableGameState::IsOnePlayerRemaining() const {
    uint32_t aliveCount{};

    for (dae::GameObject* player : m_PlayerObjects) {
        if (player == nullptr || player->IsDestroyed()) {
            continue;
        }

        dae::Reference<PlayerHealth> health = player->GetComponent<PlayerHealth>();
        if (!health || health->lives > 0) {
            ++aliveCount;
        }
    }

    return aliveCount == 1;
}

bool PlayableGameState::AreAllEnemiesDestroyed() const {
    return m_EnemyObjects.empty();
}

void PlayableGameState::CompleteLevel() {
    m_IsCompletingLevel = true;

    if (m_CurrentLevelIndex >= 2) {
        EndGame();
        return;
    }

    LoadLevel(m_CurrentLevelIndex + 1);
}

void PlayableGameState::EndGame() {
    m_IsCompletingLevel = true;
    Game::GetInstance().LoadGameEnd();
}
