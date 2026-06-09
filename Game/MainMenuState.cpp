//
// Created by frane on 5/27/2026.
//

#include "MainMenuState.h"

#include <iostream>
#include <utility>

#include "GameTime.h"
#include "PlayerInputHandler.h"
#include "PlayerInputManager.h"
#include "SceneManager.h"
#include "Button.h"
#include "Game.h"
#include "Minigin.h"

namespace dae {
    class Scene;
}

void MainMenuState::CreateButton(const std::string &text, std::function<void()> callback) {
    std::unique_ptr<dae::GameObject> gameobject = std::make_unique<dae::GameObject>();
    dae::Reference<Button> button = gameobject->AddComponent<Button>();
    button->SetText(text);
    button->SetCallback(std::move(callback));
    button->UpdateTexture();
    m_Scene->Add(std::move(gameobject));

    m_Buttons.push_back(button);
}

void MainMenuState::CreateInfoTexts() {

    glm::vec2 windowSize{GetWindowSize()};
    glm::vec2 center{windowSize / 2.0f};

    {
        std::unique_ptr<dae::GameObject> gameobject = std::make_unique<dae::GameObject>();
        dae::Reference<TextRenderer> titleRenderer = gameobject->AddComponent<TextRenderer>();
        titleRenderer->SetText("Tron : Battle Tanks");
        titleRenderer->UpdateTexture();
        glm::vec2 textSize{titleRenderer->GetSize()};

        gameobject->transform.SetWorldPosition(center.x - textSize.x / 2.0f, 110.0f);

        m_Scene->Add(std::move(gameobject));
    }

    {
        std::unique_ptr<dae::GameObject> gameobject = std::make_unique<dae::GameObject>();
        m_ErrorText = gameobject->AddComponent<TextRenderer>();
        m_ErrorText->SetColor({ 255, 0, 0, 255 });
        m_ErrorText->UpdateTexture();

        glm::vec2 textSize{m_ErrorText->GetSize()};

        gameobject->transform.SetWorldPosition({ center.x - textSize.x / 2.0f, windowSize.y - textSize.y, 0.0f });

        m_Scene->Add(std::move(gameobject));
    }

    {
        std::unique_ptr<dae::GameObject> gameobject = std::make_unique<dae::GameObject>();
        m_PlayerText0 = gameobject->AddComponent<TextRenderer>();

        if (PlayerInputManager::GetInstance().GetPlayers().empty()) {
            m_PlayerText0->SetText("Press [Space], [LCTRL] or [GAMEPAD_A] to join");
        }

        gameobject->transform.SetWorldPosition({0.0f, 0.0f, 0.0f});

        m_Scene->Add(std::move(gameobject));
    }

    {
        std::unique_ptr<dae::GameObject> gameobject = std::make_unique<dae::GameObject>();
        m_PlayerText1 = gameobject->AddComponent<TextRenderer>();

        gameobject->transform.SetWorldPosition({0.0f, Game::GetInstance().FONT_SIZE, 0.0f});

        m_Scene->Add(std::move(gameobject));
    }

    if (!PlayerInputManager::GetInstance().GetPlayers().empty()) {
        UpdatePlayerInfoText();
    }
}

void MainMenuState::UpdateButtonLocations() {
    if (m_Buttons.empty())
        return;

    glm::vec2 windowSize = GetWindowSize();

    glm::vec2 center{ windowSize / 2.0f };

    const int font_size{Game::GetInstance().FONT_SIZE};

    const float verticalOffset { m_Buttons.size() / 2.0f * font_size };
    uint32_t index{0};
    for (dae::Reference<Button>& button : m_Buttons) {
        if (button) {
            glm::vec2 buttonSize{button->GetSize()};
            glm::vec2 buttonSizeHalf{buttonSize / 2.0f};

            glm::vec2 position{center - buttonSizeHalf};

            button->GetParent()->transform.SetWorldPosition({ position.x, position.y - verticalOffset + index * font_size, 0.0f });
        }
        index++;
    }
}

void MainMenuState::UpdatePlayerInfoText() {
    if (!m_PlayerText0 || !m_PlayerText1)
        return;
    const std::vector<PlayerInputHandler>& players{PlayerInputManager::GetInstance().GetPlayers()};
    if (players.size() == 0) {
        m_PlayerText0->SetText("Press [Space], [LCTRL] or [GAMEPAD_A] to join");
        m_PlayerText1->SetText("");
    }
    else if (players.size() == 1) {
        const PlayerInputHandler& player{players[0]};
        std::string playerKeyset{};
        if (player.IsGamepad()) {
            playerKeyset = "DPAD/Left Joystick (Move) + A (Shoot)";
        }
        else {
            if (player.GetPlayerIndex() == 0) {
                playerKeyset = "WSAD (Move) + Space (Shoot)";
            }
            else {
                playerKeyset = "Arrow keys (Move) + LCTRL (Shoot)";
            }
        }

        m_PlayerText0->SetText("1. [GameMaster] Player using " + playerKeyset);
        m_PlayerText1->SetText("");
    }
    else {
        const PlayerInputHandler& player0{players[0]};
        const PlayerInputHandler& player1{players[1]};
        std::string playerKeyset0{};
        std::string playerKeyset1{};
        if (player0.IsGamepad()) {
            playerKeyset0 = "DPAD/Left Joystick (Move) + A (Shoot)";
        }
        else {
            if (player0.GetPlayerIndex() == 0) {
                playerKeyset0 = "WSAD (Move) + Space (Shoot)";
            }
            else {
                playerKeyset0 = "Arrow keys (Move) + LCTRL (Shoot)";
            }
        }

        if (player1.IsGamepad()) {
            playerKeyset1 = "DPAD/Left Joystick (Move) + A (Shoot)";
        }
        else {
            if (player1.GetPlayerIndex() == 0) {
                playerKeyset1 = "WSAD (Move) + Space (Shoot)";
            }
            else {
                playerKeyset1 = "Arrow keys (Move) + LCTRL (Shoot)";
            }
        }

        m_PlayerText0->SetText("1. [GameMaster] Player using " + playerKeyset0);
        m_PlayerText1->SetText("2. Player using " + playerKeyset1);
    }
}

glm::vec2 MainMenuState::GetWindowSize() const {
    SDL_Window* handle = dae::Minigin::GetWindow();
    int width, height;
    SDL_GetWindowSize(handle, &width, &height);

    return { width, height};
}

void MainMenuState::SetErrorText(const std::string &text) {
    if (m_ErrorText) {
        glm::vec2 windowSize{GetWindowSize()};
        glm::vec2 center{windowSize / 2.0f};

        m_ErrorText->SetText(text);
        m_ErrorText->UpdateTexture();

        glm::vec2 textSize{m_ErrorText->GetSize()};

        m_ErrorText->GetParent()->transform.SetWorldPosition({ center.x - textSize.x / 2.0f, windowSize.y - textSize.y, 0.0f });
    }
}

void MainMenuState::PlayerJoinCallback(PlayerInputHandler &handler) {
    UpdatePlayerInfoText();
}

void MainMenuState::Enter() {
    // Load main menu scene
    m_Scene = dae::SceneManager::GetInstance().CreateScene();

    PlayerInputManager::GetInstance().SetPlayerJoinCallback([&](PlayerInputHandler& handler) { this->PlayerJoinCallback(handler); });

    CreateButton("SinglePlayer", []() { Game::GetInstance().LoadSinglePlayer(); });
    CreateButton("Coop", []() { Game::GetInstance().LoadCoopPlayer(); });
    CreateButton("Versus", []() { Game::GetInstance().LoadVersusPlayer(); });
    CreateButton("Quit", []() { dae::Minigin::GetInstance().Stop(); });

    UpdateButtonLocations();

    CreateInfoTexts();
}

void MainMenuState::Exit() {
    dae::SceneManager::GetInstance().UnloadScene(m_Scene);
    m_Scene = nullptr;
    m_Buttons.clear();

    PlayerInputManager::GetInstance().SetPlayerJoinCallback(nullptr);
}

void MainMenuState::Update() {
    const float time = dae::GameTime::GetInstance().GetTime();

    for (int index = 0; index < m_Buttons.size(); ++index) {
        m_Buttons[index]->SetSelected(index == m_ButtonSelection);
    }

    if (m_LastInputTime + m_InputDelay < time) {
        const std::vector<PlayerInputHandler>& players = PlayerInputManager::GetInstance().GetPlayers();

        if (players.empty())
            return;

        const PlayerInputHandler& mainPlayer = players[0];

        if (mainPlayer.GetStartTime() + 0.5f > time)
            return;

        if (mainPlayer.IsShooting()) {
            m_Buttons[m_ButtonSelection]->Execute();
            return;
        }

        float y = mainPlayer.GetMove().y;
        if (y > 0.5f) {
            m_ButtonSelection--;
        }
        else if (y < -0.5f) {
            m_ButtonSelection++;
        }
        else {
            return;
        }

        m_LastInputTime = time;

        if (m_ButtonSelection >= m_Buttons.size()) {
            m_ButtonSelection = 0;
        }
        else if (m_ButtonSelection < 0) {
            m_ButtonSelection = m_Buttons.size() - 1;
        }
    }
}

void MainMenuState::Render() {

}
