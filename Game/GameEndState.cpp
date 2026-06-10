//
// Created by frane on 6/10/2026.
//

#include "GameEndState.h"

#include <memory>

#include "GameObject.h"
#include "HighScoreBeatenDisplay.h"
#include "HighScoreTableDisplay.h"
#include "Minigin.h"
#include "PlayerInputManager.h"
#include "SceneManager.h"
#include "TextRenderer.h"

void GameEndState::Update() {
}

void GameEndState::Render() {
}

void GameEndState::Enter() {
    m_Scene = dae::SceneManager::GetInstance().CreateScene();

    glm::vec2 windowSize = dae::Minigin::GetInstance().GetWindowSize();

    {
        std::unique_ptr<dae::GameObject> gameobject = std::make_unique<dae::GameObject>();
        gameobject->AddComponent<TextRenderer>();
        gameobject->AddComponent<HighScoreTableDisplay>();

        m_Scene->Add(std::move(gameobject));
    }

    {
        std::unique_ptr<dae::GameObject> gameobject = std::make_unique<dae::GameObject>();
        gameobject->AddComponent<TextRenderer>();
        dae::Reference<HighScoreBeatenDisplay> display = gameobject->AddComponent<HighScoreBeatenDisplay>();
        display->SetPlayerIndex(0);
        display->SetTextWhenBeaten("NEW HIGH SCORE - ENTER NAME");
        display->SetSubmitDelay(0.75f);
        display->SetMoveRepeatDelay(0.1f);
        display->SetNameLength(6);

        gameobject->transform.SetWorldPosition(windowSize.x / 2.0f, windowSize.y / 2.0f - 50.0f);

        m_Scene->Add(std::move(gameobject));
    }
    if (PlayerInputManager::GetInstance().GetPlayers().size() == 2) {
        std::unique_ptr<dae::GameObject> gameobject = std::make_unique<dae::GameObject>();
        gameobject->AddComponent<TextRenderer>();
        dae::Reference<HighScoreBeatenDisplay> display = gameobject->AddComponent<HighScoreBeatenDisplay>();
        display->SetPlayerIndex(1);
        display->SetTextWhenBeaten("NEW HIGH SCORE - ENTER NAME");
        display->SetSubmitDelay(0.75f);
        display->SetMoveRepeatDelay(0.1f);
        display->SetNameLength(6);

        gameobject->transform.SetWorldPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

        m_Scene->Add(std::move(gameobject));
    }
}

void GameEndState::Exit() {
    if (m_Scene != nullptr) {
        dae::SceneManager::GetInstance().UnloadScene(m_Scene);
        m_Scene = nullptr;
    }
}
