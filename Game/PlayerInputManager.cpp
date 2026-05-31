//
// Created by frane on 5/31/2026.
//

#include "PlayerInputManager.h"

#include <iostream>

#include "Input.h"

void PlayerInputManager::initialize() {
    std::cout << "Initialized player input manager" << std::endl;
}

void PlayerInputManager::update() {

}

const std::vector<PlayerInputHandler> & PlayerInputManager::getPlayers() const {
    return m_Players;
}

PlayerInputManager::PlayerInputManager() {
    m_AnyKeyCallbackId = dae::Input::getAnyPerformed().addListener([&](const dae::CommandContext& ctx) {
        if (m_Players.size() >= 2) // Max 2 players in the game
            return;
        if (ctx.type == dae::CommandType::KEY_PRESS) {
            std::cout << "Keypress" << std::endl;
            if (!m_HasKeyPlayer0 && ctx.scanCode == SDL_SCANCODE_SPACE) {
                m_HasKeyPlayer0 = true;
                m_Players.emplace_back(0, 0, false);
            }
            if (!m_HasKeyPlayer1 && ctx.scanCode == SDL_SCANCODE_RCTRL) {
                m_HasKeyPlayer1 = true;
                m_Players.emplace_back(0, 1, false);
            }
        }
        else if (ctx.type == dae::CommandType::GAMEPAD_BUTTON_PRESS) {
            std::cout << "Gamepad" << std::endl;
            if (ctx.gamepadIndex == 0 && !m_HasGamepadPlayer0 && ctx.gamepadButton == dae::GamepadButton::A) {
                m_HasGamepadPlayer0 = true;
                m_Players.emplace_back(0, 0, true);
            }
            if (ctx.gamepadIndex == 1 && !m_HasGamepadPlayer1 && ctx.gamepadButton == dae::GamepadButton::A) {
                m_HasGamepadPlayer1 = true;
                m_Players.emplace_back(1, 1, true);
            }
        }
    });
}

PlayerInputManager::~PlayerInputManager() {
    dae::Input::getAnyPerformed().removeListener(m_AnyKeyCallbackId);
}
