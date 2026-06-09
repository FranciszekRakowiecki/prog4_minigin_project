//
// Created by frane on 6/9/2026.
//

#include "PlayableGameState.h"

#include "Game.h"
#include "GameObject.h"
#include "LevelRenderer.h"
#include "Minigin.h"
#include "Scene.h"
#include "glm/vec2.hpp"

void PlayableGameState::LoadLevel0(dae::Scene *scene) {
    LoadLevelWithData(scene, Game::GetInstance().GetLevel0());
}

void PlayableGameState::LoadLevel1(dae::Scene *scene) {
    LoadLevelWithData(scene, Game::GetInstance().GetLevel1());
}

void PlayableGameState::LoadLevel2(dae::Scene *scene) {
    LoadLevelWithData(scene, Game::GetInstance().GetLevel2());
}

void PlayableGameState::LoadLevelWithData(dae::Scene *scene, LevelData *data) {
    glm::vec2 windowSize{dae::Minigin::GetInstance().GetWindowSize()};

    {
        std::unique_ptr<dae::GameObject> gameobject{std::make_unique<dae::GameObject>()};
        dae::Reference<LevelRenderer> renderer{gameobject->AddComponent<LevelRenderer>()};

        renderer->SetLevelData(data);

        const float levelResolution{renderer->GetResolution()};

        gameobject->transform.SetWorldPosition(windowSize.x / 2.0f - levelResolution / 2.0f, 100.0f);

        scene->Add(std::move(gameobject));
    }
}
