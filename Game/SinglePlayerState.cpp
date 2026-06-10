//
// Created by frane on 5/27/2026.
//

#include "SinglePlayerState.h"

#include "Game.h"
#include "LevelRenderer.h"
#include "Minigin.h"
#include "SceneManager.h"
#include "TankController.h"
#include "TankRenderer.h"

void SinglePlayerState::Enter() {
    m_Scene = dae::SceneManager::GetInstance().CreateScene();
    LoadLevel0(m_Scene);


}

void SinglePlayerState::Exit() {
    dae::SceneManager::GetInstance().UnloadScene(m_Scene);
    m_Scene = nullptr;
}

void SinglePlayerState::OnLevelSkip() {

}
