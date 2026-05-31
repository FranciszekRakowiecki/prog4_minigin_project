//
// Created by frane on 5/27/2026.
//

#include "MainMenuState.h"

#include "SceneManager.h"

namespace dae {
    class Scene;
}

void MainMenuState::Enter() {
    // Load main menu scene
    m_Scene = dae::SceneManager::GetInstance().CreateScene();


}

void MainMenuState::Exit() {

}

void MainMenuState::Render() {

}
