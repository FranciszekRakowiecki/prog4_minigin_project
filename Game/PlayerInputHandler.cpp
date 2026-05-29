//
// Created by frane on 5/29/2026.
//

#include "PlayerInputHandler.h"

#include <algorithm>

#include "Input.h"

struct GamepadKeyMoveInput : public PlayerInputHandler::MoveInput {
    uint32_t gamepad;
    glm::vec2 move;
    dae::InputKey* moveUpKey{nullptr};
    dae::InputKey* moveDownKey{nullptr};
    dae::InputKey* moveLeftKey{nullptr};
    dae::InputKey* moveRightKey{nullptr};

    dae::InputAxis* gamepadMove{nullptr};

    dae::InputGamepadButton* moveUpGamepad{nullptr};
    dae::InputGamepadButton* moveDownGamepad{nullptr};
    dae::InputGamepadButton* moveLeftGamepad{nullptr};
    dae::InputGamepadButton* moveRightGamepad{nullptr};

    GamepadKeyMoveInput(uint32_t gamepad);
    const glm::vec2 & getMove() override;

    void keyInput();
    void gamepadButtonInput();
    void gamepadAxisInput();

    void update() override;
};

void GamepadKeyMoveInput::keyInput() {
    move.y += moveUpKey->isPressed() ? 1.0f : 0.0f;
    move.y += moveDownKey->isPressed() ? -1.0f : 0.0f;

    move.x += moveRightKey->isPressed() ? 1.0f : 0.0f;
    move.x += moveLeftKey->isPressed() ? -1.0f : 0.0f;
}

void GamepadKeyMoveInput::gamepadButtonInput() {
    move.y += moveUpGamepad->isPressed() ? 1.0f : 0.0f;
    move.y += moveDownGamepad->isPressed() ? -1.0f : 0.0f;

    move.x += moveRightGamepad->isPressed() ? 1.0f : 0.0f;
    move.x += moveLeftGamepad->isPressed() ? -1.0f : 0.0f;
}

void GamepadKeyMoveInput::gamepadAxisInput() {
    move.x += gamepadMove->getX();
    move.y += gamepadMove->getX();
}

void GamepadKeyMoveInput::update() {
    keyInput();
    gamepadButtonInput();
    gamepadAxisInput();

    move.x = std::clamp(move.x, -1.0f, 1.0f);
    move.y = std::clamp(move.y, -1.0f, 1.0f);
}

GamepadKeyMoveInput::GamepadKeyMoveInput(uint32_t gamepad) : MoveInput(gamepad), gamepad(gamepad) {
    moveUpKey = dae::Input::KEY(SDL_SCANCODE_W);
    moveDownKey = dae::Input::KEY(SDL_SCANCODE_S);
    moveLeftKey = dae::Input::KEY(SDL_SCANCODE_A);
    moveRightKey = dae::Input::KEY(SDL_SCANCODE_D);

    gamepadMove = dae::Input::AXIS(dae::InputAxisType::GAMEPAD_LEFT);

    moveUpGamepad = dae::Input::GAMEPAD_BUTTON(dae::GamepadButton::DPAD_UP);
    moveDownGamepad = dae::Input::GAMEPAD_BUTTON(dae::GamepadButton::DPAD_DOWN);
    moveLeftGamepad = dae::Input::GAMEPAD_BUTTON(dae::GamepadButton::DPAD_LEFT);
    moveRightGamepad = dae::Input::GAMEPAD_BUTTON(dae::GamepadButton::DPAD_RIGHT);
}

const glm::vec2 & GamepadKeyMoveInput::getMove() {
    return move;
}

PlayerInputHandler::MoveInput::MoveInput(uint32_t gamepad) {
}

PlayerInputHandler::PlayerInputHandler(uint32_t gamepadIndex) : m_GamepadIndex(gamepadIndex), m_MoveInput(std::make_unique<GamepadKeyMoveInput>(gamepadIndex)) {

}

void PlayerInputHandler::update() {
    m_MoveInput->update();

    m_Move = m_MoveInput->getMove();
}
