//
// Created by frane on 5/6/2026.
//

#include "Game.h"

#include <iostream>

#include "LevelLoader.h"
#include "Minigin.h"
#include "PlayerInputManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "ServiceLocator.h"
#include "SoundSystemSDL.h"
#include "TextObject.h"

Game::Game() :
m_NullGameState(std::make_unique<NullGameState>()),
m_MainMenuState(std::make_unique<MainMenuState>()),
m_SinglePlayerState(std::make_unique<SinglePlayerState>()),
m_CoopPlayerState(std::make_unique<CoopPlayerState>()),
m_VersusPlayerState(std::make_unique<VersusPlayerState>()),
m_GameEndState(std::make_unique<GameEndState>()),
m_GameState(nullptr),
m_SkipLevelKey(dae::Input::KEY(SDL_SCANCODE_F1)),
m_MuteKey(dae::Input::KEY(SDL_SCANCODE_F2))
{
    m_GameState = m_NullGameState.get();
}

void Game::ChangeState(GameState *state) {
    m_GameState->Exit();
    if (state == nullptr) {
        m_GameState = m_NullGameState.get();
        return;
    }
    m_GameState = state;
    m_GameState->Enter();
}

void Game::RequestStateChange(GameState* state) {
    m_PendingGameState = state;
    m_HasPendingGameState = true;
}

void Game::ApplyPendingStateChange() {
    if (!m_HasPendingGameState) {
        return;
    }

    GameState* nextState = m_PendingGameState;
    m_PendingGameState = nullptr;
    m_HasPendingGameState = false;

    ChangeState(nextState);
}

void Game::LoadAudio() {
    SoundSystem& sound = ServiceLocator::GetInstance().getSoundSystem();
    m_ButtonSelectSFX = sound.registerSound(MINIGIN_DATA_PATH + "buttonSelect.wav");
    m_EnemyShootSFX = sound.registerSound(MINIGIN_DATA_PATH + "enemyShoot.wav");
    m_PlayerShootSFX = sound.registerSound(MINIGIN_DATA_PATH + "playerShoot.wav");
    m_EnemyDeathSFX = sound.registerSound(MINIGIN_DATA_PATH + "enemyDeath.wav");
    m_PlayerDeathSFX = sound.registerSound(MINIGIN_DATA_PATH + "playerDeath.wav");
    m_EnterLevelSFX = sound.registerSound(MINIGIN_DATA_PATH + "enterLevel.wav");
}

void Game::Start() {
    ServiceLocator::GetInstance().setSoundSystem(std::make_unique<SoundSystemSDL>());

    m_GameFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", FONT_SIZE);
    m_ScoreKeeper.LoadHighScores();

    PlayerInputManager::GetInstance().Initialize();

    LoadAudio();

    // auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
    // auto fps = std::make_unique<dae::TextObject>("FPS: ", font);
    // fps->SetColor({ 255, 255, 255, 255 });
    // fps->SetPosition(0, 0);
    // fps->AddComponent<dae::ExampleUpdateComponent>();
    //
    // scene.Add(std::move(fps));

    m_Level0 = std::make_unique<LevelData>(std::move(LevelLoader::LoadFromFile("TronLevel1.tlvl")));
    m_Level1 = std::make_unique<LevelData>(std::move(LevelLoader::LoadFromFile("TronLevel2.tlvl")));
    m_Level2 = std::make_unique<LevelData>(std::move(LevelLoader::LoadFromFile("TronLevel3.tlvl")));

    LoadMainMenu();
}

void Game::Update() {
    ApplyPendingStateChange();

    PlayerInputManager::GetInstance().Update();

    if (m_SkipLevelKey->pressedThisFrame()) {
        PlayableGameState* playable = dynamic_cast<PlayableGameState*>(m_GameState);
        if (playable != nullptr) {
            playable->OnLevelSkip();
        }
    }

    if (m_MuteKey->pressedThisFrame()) {
        m_IsGameMuted = !m_IsGameMuted;
        if (m_IsGameMuted) {
            ServiceLocator::GetInstance().setSoundSystem(nullptr);
        }
        else {
            ServiceLocator::GetInstance().setSoundSystem(std::make_unique<SoundSystemSDL>());
            LoadAudio();
        }
    }

    m_GameState->Update();
}

void Game::Render() {
    m_GameState->Render();
}

void Game::LoadMainMenu() {
    RequestStateChange(m_MainMenuState.get());
}

void Game::LoadSinglePlayer() {
    RequestStateChange(m_SinglePlayerState.get());
}

void Game::LoadCoopPlayer() {
    if (PlayerInputManager::GetInstance().GetPlayers().size() != 2) {
        // Update text in main menu to show error
        m_MainMenuState->SetErrorText("Coop mode requires at least 2 players.");
        return;
    }
    RequestStateChange(m_CoopPlayerState.get());
}

void Game::LoadVersusPlayer() {
    if (PlayerInputManager::GetInstance().GetPlayers().size() != 2) {
        // Update text in main menu to show error
        m_MainMenuState->SetErrorText("Versus mode requires at least 2 players.");
        return;
    }
    RequestStateChange(m_VersusPlayerState.get());
}

void Game::LoadGameEnd() {
    RequestStateChange(m_GameEndState.get());
}

bool Game::IsInMainMenu() const {
    return m_GameState == m_MainMenuState.get();
}

LevelData * Game::GetLevel0() const {
    return m_Level0.get();
}

LevelData * Game::GetLevel1() const {
    return m_Level1.get();
}

LevelData * Game::GetLevel2() const {
    return m_Level2.get();
}

std::shared_ptr<dae::Font> Game::GetGameFont() const {
    return m_GameFont;
}

ScoreKeeper& Game::GetScoreKeeper() {
    return m_ScoreKeeper;
}

GameState * Game::GetGameState() const {
    return m_GameState;
}

void Game::PlaySFX(GameSFX sfx) {
    SoundSystem& sound = ServiceLocator::GetInstance().getSoundSystem();
    switch (sfx) {
        case GameSFX::PLAYER_SHOOT:
            sound.playSound(m_PlayerShootSFX, 0.2f);
            break;
        case GameSFX::PLAYER_DEATH:
            sound.playSound(m_PlayerDeathSFX, 0.4f);
            break;
        case GameSFX::ENTER_LEVEL:
            sound.playSound(m_EnterLevelSFX, 0.5f);
            break;
        case GameSFX::ENEMY_SHOOT:
            sound.playSound(m_EnemyShootSFX, 0.2f);
            break;
        case GameSFX::ENEMY_DEATH:
            sound.playSound(m_EnemyDeathSFX, 0.4f);
            break;
        case GameSFX::BUTTON_CLICK:
            sound.playSound(m_ButtonSelectSFX, 0.2f);
            break;
    }
}
