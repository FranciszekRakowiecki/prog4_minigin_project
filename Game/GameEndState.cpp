//
// Created by frane on 6/10/2026.
//

#include "GameEndState.h"

#include <memory>
#include <vector>

#include "Game.h"
#include "GameTime.h"
#include "GameObject.h"
#include "HighScoreBeatenDisplay.h"
#include "HighScoreTableDisplay.h"
#include "Minigin.h"
#include "PlayerInputHandler.h"
#include "PlayerInputManager.h"
#include "ScoreKeeper.h"
#include "SceneManager.h"
#include "TextRenderer.h"

void GameEndState::Update() {
    if (HasPendingHighScore()) {
        return;
    }

    if (m_ReturnDelayTimer > 0.0f) {
        m_ReturnDelayTimer -= dae::GameTime::GetInstance().GetDeltaTime();
        return;
    }

    if (IsAnyPlayerSubmitting()) {
        Game::GetInstance().LoadMainMenu();
    }
}

void GameEndState::Render() {
}

void GameEndState::Enter() {
    m_Scene = dae::SceneManager::GetInstance().CreateScene();
    m_ReturnDelayTimer = m_ReturnDelay;

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

    if (!HasPendingHighScore()) {
        std::unique_ptr<dae::GameObject> gameobject = std::make_unique<dae::GameObject>();
        dae::Reference<TextRenderer> text = gameobject->AddComponent<TextRenderer>();
        text->SetText("SHOOT TO RETURN TO MENU");

        gameobject->transform.SetWorldPosition(windowSize.x / 2.0f - 180.0f, windowSize.y - 80.0f);

        m_Scene->Add(std::move(gameobject));
    }
}

void GameEndState::Exit() {
    if (m_Scene != nullptr) {
        dae::SceneManager::GetInstance().UnloadScene(m_Scene);
        m_Scene = nullptr;
    }
}

bool GameEndState::HasPendingHighScore() const {
    const std::vector<PlayerInputHandler>& players = PlayerInputManager::GetInstance().GetPlayers();
    ScoreKeeper& scoreKeeper = Game::GetInstance().GetScoreKeeper();

    for (const PlayerInputHandler& player : players) {
        if (scoreKeeper.IsPlayerHighScore(player.GetPlayerIndex())) {
            return true;
        }
    }

    return false;
}

bool GameEndState::IsAnyPlayerSubmitting() const {
    const std::vector<PlayerInputHandler>& players = PlayerInputManager::GetInstance().GetPlayers();
    for (const PlayerInputHandler& player : players) {
        if (player.IsShooting()) {
            return true;
        }
    }

    return false;
}
