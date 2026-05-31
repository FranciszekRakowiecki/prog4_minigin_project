//
// Created by frane on 5/29/2026.
//

#include "PlayerInputHandler.h"

#include <algorithm>
#include <iostream>

#include "Input.h"

struct KeyMoveInput : public PlayerInputHandler::MoveInput {
    uint32_t gamepad;
    glm::vec2 move;
    dae::InputKey* moveUpKey{nullptr};
    dae::InputKey* moveDownKey{nullptr};
    dae::InputKey* moveLeftKey{nullptr};
    dae::InputKey* moveRightKey{nullptr};

    explicit KeyMoveInput(uint32_t gamepad, uint32_t playerIndex);
    const glm::vec2 & getMove() override;

    void keyInput();

    void update() override;
};

struct GamepadMoveInput : public PlayerInputHandler::MoveInput {
    dae::InputAxis* gamepadMove{nullptr};
    glm::vec2 move{0.0f}; // Doesnt actually matter where i put it, itll align 8 anyway

    dae::InputGamepadButton* moveUpGamepad{nullptr};
    dae::InputGamepadButton* moveDownGamepad{nullptr};
    dae::InputGamepadButton* moveLeftGamepad{nullptr};
    dae::InputGamepadButton* moveRightGamepad{nullptr};

    explicit GamepadMoveInput(uint32_t gamepad, uint32_t playerIndex);

    const glm::vec2 & getMove() override;

    void gamepadButtonInput();
    void gamepadAxisInput();

    void update() override;
};

GamepadMoveInput::GamepadMoveInput(uint32_t gamepad, uint32_t playerIndex) : MoveInput(gamepad, playerIndex) {
    gamepadMove = dae::Input::AXIS(gamepad, dae::InputAxisType::GAMEPAD_LEFT);

    moveUpGamepad = dae::Input::GAMEPAD_BUTTON(gamepad, dae::GamepadButton::DPAD_UP);
    moveDownGamepad = dae::Input::GAMEPAD_BUTTON(gamepad, dae::GamepadButton::DPAD_DOWN);
    moveLeftGamepad = dae::Input::GAMEPAD_BUTTON(gamepad, dae::GamepadButton::DPAD_LEFT);
    moveRightGamepad = dae::Input::GAMEPAD_BUTTON(gamepad, dae::GamepadButton::DPAD_RIGHT);

    if (gamepad == 0)
        std::cout << "Created player 1 for gamepad" << std::endl;
    else {
        std::cout << "Created player 2 for gamepad" << std::endl;
    }
}

const glm::vec2 & GamepadMoveInput::getMove() {
    return move;
}

void GamepadMoveInput::update() {
    move = {};
    gamepadButtonInput();
    gamepadAxisInput();

    move.x = std::clamp(move.x, -1.0f, 1.0f);
    move.y = std::clamp(move.y, -1.0f, 1.0f);
}

struct KeyShootInput : public PlayerInputHandler::ShootInput {
    bool shooting{false};

    dae::InputKey* shootKey{nullptr};

    explicit KeyShootInput(uint32_t gamepad, uint32_t playerIndex);

    bool isShooting() const override;

    void update() override;
};

struct GamepadShootInput : public PlayerInputHandler::ShootInput {
    bool shooting{false};
    uint32_t gamepad{0};
    dae::InputGamepadButton* shootGamepad{nullptr};

    explicit GamepadShootInput(uint32_t gamepad, uint32_t playerIndex);

    bool isShooting() const override;

    void update() override;
};

GamepadShootInput::GamepadShootInput(uint32_t gamepad, uint32_t playerIndex) : ShootInput(gamepad, playerIndex), gamepad(gamepad) {
    shootGamepad = dae::Input::GAMEPAD_BUTTON(gamepad, dae::GamepadButton::A);
}

bool GamepadShootInput::isShooting() const {
    return shooting;
}

void GamepadShootInput::update() {
    shooting = shootGamepad->isPressed();
}

KeyMoveInput::KeyMoveInput(uint32_t gamepad, uint32_t playerIndex) : MoveInput(gamepad, playerIndex), gamepad(gamepad) {
    if (playerIndex == 0) {
        moveUpKey = dae::Input::KEY(SDL_SCANCODE_W);
        moveDownKey = dae::Input::KEY(SDL_SCANCODE_S);
        moveLeftKey = dae::Input::KEY(SDL_SCANCODE_A);
        moveRightKey = dae::Input::KEY(SDL_SCANCODE_D);
        std::cout << "Created player 1 for keyboard" << std::endl;
    }
    else {
        moveUpKey = dae::Input::KEY(SDL_SCANCODE_UP);
        moveDownKey = dae::Input::KEY(SDL_SCANCODE_DOWN);
        moveLeftKey = dae::Input::KEY(SDL_SCANCODE_LEFT);
        moveRightKey = dae::Input::KEY(SDL_SCANCODE_RIGHT);
        std::cout << "Created player 2 for keyboard" << std::endl;
    }
}

const glm::vec2 & KeyMoveInput::getMove() {
    return move;
}

void KeyMoveInput::keyInput() {
    move.y += moveUpKey->isPressed() ? 1.0f : 0.0f;
    move.y += moveDownKey->isPressed() ? -1.0f : 0.0f;

    move.x += moveRightKey->isPressed() ? 1.0f : 0.0f;
    move.x += moveLeftKey->isPressed() ? -1.0f : 0.0f;
}

void KeyMoveInput::update() {
    move = {};
    keyInput();

    move.x = std::clamp(move.x, -1.0f, 1.0f);
    move.y = std::clamp(move.y, -1.0f, 1.0f);
}

void GamepadMoveInput::gamepadButtonInput() {
    move.y += moveUpGamepad->isPressed() ? 1.0f : 0.0f;
    move.y += moveDownGamepad->isPressed() ? -1.0f : 0.0f;

    move.x += moveRightGamepad->isPressed() ? 1.0f : 0.0f;
    move.x += moveLeftGamepad->isPressed() ? -1.0f : 0.0f;
}

void GamepadMoveInput::gamepadAxisInput() {
    move.x += gamepadMove->getX();
    move.y += gamepadMove->getY();
}

KeyShootInput::KeyShootInput(uint32_t gamepad, uint32_t playerIndex) : ShootInput(gamepad, playerIndex) {
    if (playerIndex == 0) {
        shootKey = dae::Input::KEY(SDL_SCANCODE_SPACE);
    }
    else {
        shootKey = dae::Input::KEY(SDL_SCANCODE_RCTRL);
    }
}

bool KeyShootInput::isShooting() const {
    return shooting;
}

void KeyShootInput::update() {
    shooting = shootKey->isPressed();
}

PlayerInputHandler::PlayerInputHandler(uint32_t gamepadIndex, uint32_t playerIndex, bool isGamepad) :
m_GamepadIndex(gamepadIndex),
m_MoveInput(nullptr) {
    if (isGamepad) {
        m_MoveInput = std::make_unique<GamepadMoveInput>(gamepadIndex, playerIndex);
        m_ShootInput = std::make_unique<GamepadShootInput>(gamepadIndex, playerIndex);
    }
    else {
        m_MoveInput = std::make_unique<KeyMoveInput>(gamepadIndex, playerIndex);
        m_ShootInput = std::make_unique<KeyShootInput>(gamepadIndex, playerIndex);
    }
}

void PlayerInputHandler::update() {
    m_MoveInput->update();
    m_ShootInput->update();

    m_Move = m_MoveInput->getMove();
    m_IsShooting = m_ShootInput->isShooting();
}
