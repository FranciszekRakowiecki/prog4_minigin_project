//
// Created by frane on 6/9/2026.
//

#include "PlayableGameState.h"

#include <iostream>

#include "Game.h"
#include "GameObject.h"
#include "LevelRenderer.h"
#include "Minigin.h"
#include "PlayerHealth.h"
#include "Scene.h"
#include "TankController.h"
#include "TankRenderer.h"
#include "glm/vec2.hpp"

void PlayableGameState::LoadLevel0(dae::Scene *scene) {
    LoadLevelWithData(scene, Game::GetInstance().GetLevel0(), 0);
}

void PlayableGameState::LoadLevel1(dae::Scene *scene) {
    LoadLevelWithData(scene, Game::GetInstance().GetLevel1(), 1);
}

void PlayableGameState::LoadLevel2(dae::Scene *scene) {
    LoadLevelWithData(scene, Game::GetInstance().GetLevel2(), 2);
}

void PlayableGameState::LoadLevelWithData(dae::Scene *scene, const LevelData *data, uint32_t index) {
    UnloadCurrentScene();

    m_CurrentLevel = data;
    m_CurrentLevelIndex = index;

    std::unique_ptr<dae::GameObject> gameobject{std::make_unique<dae::GameObject>()};
    dae::Reference<LevelRenderer> renderer{gameobject->AddComponent<LevelRenderer>()};

    renderer->SetLevelData(data);

    gameobject->transform.SetWorldPosition(TiletoWorld(0, 0, data));

    scene->Add(std::move(gameobject));

    OnLevelLoad(scene, data, index);
}

void PlayableGameState::SpawnPlayer(dae::Scene *scene, const LevelData *data, const PlayerInputHandler *handler) {
    std::unique_ptr<dae::GameObject> gameobject = std::make_unique<dae::GameObject>();
    dae::Reference<TankRenderer> tank_renderer = gameobject->AddComponent<TankRenderer>();
    dae::Reference<TankController> tank_controller = gameobject->AddComponent<TankController>();

    tank_controller->SetPlayerInput(handler);

    RespawnPlayer(gameobject.get(), data);

    scene->Add(std::move(gameobject));
}

void PlayableGameState::SpawnEnemies(dae::Scene *scene, const LevelData *data) {

}

void PlayableGameState::RespawnPlayer(dae::GameObject *player, const LevelData *data) {
    dae::Reference<PlayerHealth> health = player->GetComponent<PlayerHealth>();
    if (health) {
        health->lives--;

        std::cout << "Player has lost a life and respawned." << std::endl;
    }
    player->transform.SetWorldPosition(TiletoWorld(data->spawns[0].tileX, data->spawns[0].tileY, data));
}

glm::vec3 PlayableGameState::TiletoWorld(uint32_t x, uint32_t y, const LevelData* data) {
    const glm::vec2 windowSize{dae::Minigin::GetInstance().GetWindowSize()};
    const float tileSize = (windowSize.y - 100.0f) / float(data->height);
    const float levelResolution{data->height * tileSize};
    const glm::vec3 levelOffset{windowSize.x / 2.0f - levelResolution / 2.0f, 0.0f, 100.0f};

    return levelOffset + glm::vec3{ x * tileSize, y * tileSize, 0.0f};
}

const LevelData * PlayableGameState::GetCurrentLevelData() const {
    return m_CurrentLevel;
}

uint32_t PlayableGameState::GetCurrentLevelIndex() const {
    return m_CurrentLevelIndex;
}
