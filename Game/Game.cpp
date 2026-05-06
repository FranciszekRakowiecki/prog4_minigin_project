//
// Created by frane on 5/6/2026.
//

#include "Game.h"

Game::Game() :
m_NullGameState(std::make_unique<NullGameState>()),
m_MainMenuState(std::make_unique<MainMenuState>()),
m_SinglePlayerState(std::make_unique<SinglePlayerState>()),
m_CoopPlayerState(std::make_unique<CoopPlayerState>()),
m_VersusPlayerState(std::make_unique<VersusPlayerState>()),
m_GameState(nullptr)
{
    m_GameState = m_NullGameState.get();
}

void Game::ChangeState(GameState *state) {
    m_GameState->Exit();
    if (state == nullptr) {
        m_GameState = m_NullGameState.get();
        return;
    }
    m_GameState->Exit();
    m_GameState = state;
    m_GameState->Enter();
}

void Game::Start() {

}

void Game::Update() {
    m_GameState->Update();
}

void Game::Render() {
    m_GameState->Render();
}

void Game::LoadMainMenu() {
    ChangeState(m_MainMenuState.get());
}

void Game::LoadSinglePlayer() {
    ChangeState(m_SinglePlayerState.get());
}

void Game::LoadCoopPlayer() {
    ChangeState(m_CoopPlayerState.get());
}

void Game::LoadVersusPlayer() {
    ChangeState(m_VersusPlayerState.get());
}
