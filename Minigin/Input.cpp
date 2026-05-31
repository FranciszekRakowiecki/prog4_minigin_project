//
// Created by frane on 3/11/2026.
//

#include "Input.h"

#include <climits>
#include <iostream>

#ifndef __EMSCRIPTEN__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Xinput.h>
#endif

#include "Minigin.h"
#include <SDL3/SDL.h>

using namespace dae;

Input* Input::Instance{nullptr};

class Input::GamepadImpl {
public:
    Input* input;
    GamepadImpl(Input* input);
    virtual ~GamepadImpl() = default;

    std::array<int, Input::MaxGamepads> gamepadButtons{};
    virtual void pollControllers(std::array<std::map<GamepadButton, InputGamepadButton*>, Input::MaxGamepads>&) {};
    virtual void pollAxis(std::array<InputAxis*, Input::MaxGamepads>&, std::array<InputAxis*, Input::MaxGamepads>&) {};
    virtual void onGamepadDown(SDL_Event&) {};
    virtual void onGamepadUp(SDL_Event&) {};
    virtual void onJoystickMove(SDL_Event&) {};
    virtual int GetGamepadButtons(int gamepadIndex) { return gamepadButtons[gamepadIndex]; }
    virtual void setGamepad(SDL_Gamepad*) {};
    virtual void removeGamepad(SDL_JoystickID) {};
    virtual void frame() {};

    void setPressed(InputGamepadButton* button, bool state) {
        input->setPressed(button, state);
    }
    void setXY(InputAxis* axis, float x, float y) {
        input->setXY(axis, x, y);
    }
};

class GamepadImplSDL : public Input::GamepadImpl {
public:
    explicit GamepadImplSDL(Input *input)
        : GamepadImpl(input) {
    }
    ~GamepadImplSDL() override;

    void onGamepadDown(SDL_Event &event) override;
    void onGamepadUp(SDL_Event &event) override;
    void onJoystickMove(SDL_Event &event) override;
    void setGamepad(SDL_Gamepad *) override;
    void removeGamepad(SDL_JoystickID) override;
    void frame() override;
    void pollAxis(std::array<InputAxis*, Input::MaxGamepads>& leftThumb, std::array<InputAxis*, Input::MaxGamepads>& rightThumb) override;

    int GPBasMask(SDL_Gamepad* pad, GamepadButton mask, SDL_GamepadButton button);
    void handleInputChanges(std::map<GamepadButton, InputGamepadButton*>& gamepadButtonActions, int changes, int state, int mask, uint32_t gamepadIndex);
    int getPlayer(SDL_JoystickID joystickId) const;

private:
    std::array<SDL_Gamepad*, Input::MaxGamepads> gamepads{};
    std::map<SDL_JoystickID, int> playerByJoystickId{};
};

GamepadImplSDL::~GamepadImplSDL() {
    for (SDL_Gamepad* gamepad : gamepads) {
        if (gamepad)
            SDL_CloseGamepad(gamepad);
    }
}

void GamepadImplSDL::onGamepadDown(SDL_Event &event) {
    SDL_Gamepad* gamepad = SDL_GetGamepadFromID(event.gbutton.which);
    int player = getPlayer(event.gbutton.which);

    if (!gamepad || player < 0)
        return;

    int state{0};
    state |= GPBasMask(gamepad, GamepadButton::DPAD_UP, SDL_GAMEPAD_BUTTON_DPAD_UP);
    state |= GPBasMask(gamepad, GamepadButton::DPAD_DOWN, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
    state |= GPBasMask(gamepad, GamepadButton::DPAD_LEFT, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
    state |= GPBasMask(gamepad, GamepadButton::DPAD_RIGHT, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
    state |= GPBasMask(gamepad, GamepadButton::A, SDL_GAMEPAD_BUTTON_SOUTH);
    state |= GPBasMask(gamepad, GamepadButton::B, SDL_GAMEPAD_BUTTON_EAST);
    state |= GPBasMask(gamepad, GamepadButton::X, SDL_GAMEPAD_BUTTON_WEST);
    state |= GPBasMask(gamepad, GamepadButton::Y, SDL_GAMEPAD_BUTTON_NORTH);
    state |= GPBasMask(gamepad, GamepadButton::START, SDL_GAMEPAD_BUTTON_START);
    state |= GPBasMask(gamepad, GamepadButton::BACK, SDL_GAMEPAD_BUTTON_BACK);
    state |= GPBasMask(gamepad, GamepadButton::LEFT_SHOULDER, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
    state |= GPBasMask(gamepad, GamepadButton::RIGHT_SHOULDER, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
    state |= GPBasMask(gamepad, GamepadButton::LEFT_THUMB, SDL_GAMEPAD_BUTTON_LEFT_STICK);
    state |= GPBasMask(gamepad, GamepadButton::RIGHT_THUMB, SDL_GAMEPAD_BUTTON_RIGHT_STICK);

    // It's at least slightly pressed, axis ranges between 0 and 32,767
    if (SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) > 200) {
        state |= (int)GamepadButton::LEFT_TRIGGER;
    }
    if (SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 200) {
        state |= (int)GamepadButton::RIGHT_TRIGGER;
    }

    int changes = state ^ gamepadButtons[player];

    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::DPAD_UP, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::DPAD_DOWN, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::DPAD_LEFT, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::DPAD_RIGHT, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::A, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::B, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::X, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::Y, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::START, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::BACK, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::LEFT_SHOULDER, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::RIGHT_SHOULDER, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::LEFT_THUMB, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::RIGHT_THUMB, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::LEFT_TRIGGER, static_cast<uint32_t>(player));
    handleInputChanges(input->gamepadButtonActions[player], changes, state, (int)GamepadButton::RIGHT_TRIGGER, static_cast<uint32_t>(player));

    gamepadButtons[player] = state;
}

void GamepadImplSDL::onGamepadUp(SDL_Event &event) {
    onGamepadDown(event);
}

void GamepadImplSDL::onJoystickMove(SDL_Event &event) {
    GamepadImpl::onJoystickMove(event);
}

void GamepadImplSDL::setGamepad(SDL_Gamepad *sdl_gamepad) {
    if (!sdl_gamepad)
        return;

    SDL_JoystickID joystickId = SDL_GetGamepadID(sdl_gamepad);
    if (playerByJoystickId.contains(joystickId))
        return;

    for (int player = 0; player < Input::MaxGamepads; ++player) {
        if (!gamepads[player]) {
            gamepads[player] = sdl_gamepad;
            playerByJoystickId[joystickId] = player;
            return;
        }
    }

    SDL_CloseGamepad(sdl_gamepad);
}

void GamepadImplSDL::removeGamepad(SDL_JoystickID joystickId) {
    if (!playerByJoystickId.contains(joystickId))
        return;

    int player = playerByJoystickId[joystickId];
    if (gamepads[player])
        SDL_CloseGamepad(gamepads[player]);

    gamepads[player] = nullptr;
    gamepadButtons[player] = 0;
    playerByJoystickId.erase(joystickId);
}

void GamepadImplSDL::frame() {
    GamepadImpl::frame();
}

void GamepadImplSDL::pollAxis(std::array<InputAxis*, Input::MaxGamepads>& leftThumb, std::array<InputAxis*, Input::MaxGamepads>& rightThumb) {
    for (int player = 0; player < Input::MaxGamepads; ++player) {
        SDL_Gamepad* gamepad = gamepads[player];
        if (!gamepad)
            continue;

        CommandContext ctx{};
        {
            float x = float(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX)) / float(SHRT_MAX);
            float y = float(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY)) / float(SHRT_MAX);
            if (x != leftThumb[player]->getX() || y != leftThumb[player]->getY()) {
                setXY(leftThumb[player], x,y);
                ctx.axis = InputAxisType::GAMEPAD_LEFT;
                ctx.axisX = leftThumb[player]->getX();
                ctx.axisY = leftThumb[player]->getY();

                leftThumb[player]->execute(ctx);
            }
        }
        {
            float x = float(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX)) / float(SHRT_MAX);
            float y = float(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY)) / float(SHRT_MAX);
            if (x != rightThumb[player]->getX() || y != rightThumb[player]->getY()) {
                setXY(rightThumb[player], x,y);
                ctx.axis = InputAxisType::GAMEPAD_RIGHT;
                ctx.axisX = rightThumb[player]->getX();
                ctx.axisY = rightThumb[player]->getY();

                rightThumb[player]->execute(ctx);
            }
        }
    }
}

int GamepadImplSDL::GPBasMask(SDL_Gamepad *pad, GamepadButton mask, SDL_GamepadButton button) {
    if (SDL_GetGamepadButton(pad, button))
        return (int)mask;
    return 0;
}

int GamepadImplSDL::getPlayer(SDL_JoystickID joystickId) const {
    auto player = playerByJoystickId.find(joystickId);
    if (player == playerByJoystickId.end())
        return -1;

    return player->second;
}

void GamepadImplSDL::handleInputChanges(std::map<GamepadButton, InputGamepadButton *> &gamepadButtonActions, int changes,
    int state, int mask, uint32_t gamepadIndex) {
    if (changes & mask) {
        GamepadButton key = (GamepadButton)mask;
        bool pressed = state & mask;
        CommandContext ctx{};
        ctx.gamepadIndex = gamepadIndex;
        ctx.gamepadButton = key;
        ctx.type = pressed ? CommandType::GAMEPAD_BUTTON_PRESS : CommandType::GAMEPAD_BUTTON_RELEASE;
        this->input->getAnyPerformed().execute(ctx);
        if (gamepadButtonActions.contains(key)) {
            InputGamepadButton *button = gamepadButtonActions[key];
            setPressed(button, pressed);
            if (pressed)
                button->buttonPress(mask);
            else
                button->buttonRelease(mask);

            button->execute(ctx);
        }
        ctx.gamepadButton = GamepadButton::NONE;
        ctx.type = CommandType::NONE;
    }
}

class GamepadImplX : public Input::GamepadImpl {
public:
    explicit GamepadImplX(Input *input)
        : GamepadImpl(input) {
    }
    void pollControllers(std::array<std::map<GamepadButton, InputGamepadButton*>, Input::MaxGamepads>& gamepadButtonActions) override;
    void pollAxis(std::array<InputAxis*, Input::MaxGamepads>& leftThumb, std::array<InputAxis*, Input::MaxGamepads>& rightThumb) override;

    void handleInputChanges(std::map<GamepadButton, InputGamepadButton*>& gamepadButtonActions, int changes, int state, int mask, uint32_t gamepadIndex);
};

Input::Input(Minigin* engine, SDL_Window* window) :
    engine(engine),
    MOUSE_DELTA{new InputAxis(InputAxisType::MOUSE_DELTA)},
    CURSOR{new InputAxis(InputAxisType::CURSOR_POSITION)},
    SCROLL_DELTA{new InputAxis(InputAxisType::SCROLL)},
    window(window),
gamepadImpl(nullptr)
{
    for (int player = 0; player < MaxGamepads; ++player) {
        LEFT_THUMB_STICKS[player] = new InputAxis(InputAxisType::GAMEPAD_LEFT);
        RIGHT_THUMB_STICKS[player] = new InputAxis(InputAxisType::GAMEPAD_RIGHT);
    }

    #ifdef __EMSCRIPTEN__
    gamepadImpl = std::make_unique<GamepadImplSDL>(this);
    #else
    gamepadImpl = std::make_unique<GamepadImplX>(this);
    #endif
    Instance = this;
}

Input::~Input() {
    delete MOUSE_DELTA;
    delete SCROLL_DELTA;
    delete CURSOR;

    for (int player = 0; player < MaxGamepads; ++player) {
        delete LEFT_THUMB_STICKS[player];
        delete RIGHT_THUMB_STICKS[player];
    }

    for (std::pair<int, InputKey*> pair : keyActions)
        delete pair.second;

    for (std::pair<int, InputButton*> pair : buttonActions)
        delete pair.second;

    for (auto& playerActions : gamepadButtonActions) {
        for (std::pair<GamepadButton, InputGamepadButton*> pair : playerActions)
            delete pair.second;

        playerActions.clear();
    }

    keyActions.clear();
    buttonActions.clear();
}

void Input::pollEvents() {
    gamepadImpl->frame();
    pollKeys();
    pollButtons();
    for (auto& playerActions : gamepadButtonActions) {
        for (std::pair<GamepadButton, InputGamepadButton*> pair : playerActions)
            pair.second->frame();
    }
    pollAxis();

    SDL_PumpEvents();

    CommandContext ctx{};
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                engine->Stop();
                return;
            case SDL_EVENT_KEY_DOWN:
                ctx.scanCode = event.key.scancode;
                ctx.type = CommandType::KEY_PRESS;
                m_OnAnyPerformed.execute(ctx);
                if (keyActions.contains(event.key.scancode)) {
                    InputKey* key = keyActions[event.key.scancode];
                    key->keyPress(event.key.scancode);

                    key->execute(ctx);
                }
                ctx.scanCode = SDL_SCANCODE_0;
                ctx.type = CommandType::NONE;
                break;
            case SDL_EVENT_KEY_UP:
                ctx.scanCode = event.key.scancode;
                ctx.type = CommandType::KEY_RELEASE;
                m_OnAnyPerformed.execute(ctx);
                if (keyActions.contains(event.key.scancode)) {
                    InputKey* key = keyActions[event.key.scancode];
                    key->keyRelease(event.key.scancode);

                    key->execute(ctx);
                }
                ctx.scanCode = SDL_SCANCODE_0;
                ctx.type = CommandType::NONE;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                ctx.button = event.button.button;
                ctx.type = CommandType::BUTTON_PRESS;
                m_OnAnyPerformed.execute(ctx);
                if (buttonActions.contains(event.button.button)) {
                    InputButton* button = buttonActions[event.button.button];
                    button->buttonPress(event.button.button);

                    button->execute(ctx);
                }
                ctx.button = 0;
                ctx.type = CommandType::NONE;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                ctx.button = event.button.button;
                ctx.type = CommandType::BUTTON_RELEASE;
                m_OnAnyPerformed.execute(ctx);
                if (buttonActions.contains(event.button.button)) {
                    InputButton* button = buttonActions[event.button.button];
                    button->buttonRelease(event.button.button);

                    button->execute(ctx);
                }
                ctx.button = 0;
                ctx.type = CommandType::NONE;
                break;
            case SDL_EVENT_GAMEPAD_ADDED: {
#ifdef __EMSCRIPTEN__
                SDL_Gamepad *gamepad = SDL_OpenGamepad(event.gdevice.which);
#else
                SDL_Gamepad *gamepad = SDL_GetGamepadFromID(event.gdevice.which);
#endif
                gamepadImpl->setGamepad(gamepad);
            }
                break;
            case SDL_EVENT_GAMEPAD_REMOVED: {
                gamepadImpl->removeGamepad(event.gdevice.which);
            }
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
                gamepadImpl->onGamepadDown(event);
            }
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_UP: {
                gamepadImpl->onGamepadUp(event);
            }
                break;
            case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
                gamepadImpl->onJoystickMove(event);
            }
                break;
            case SDL_EVENT_MOUSE_WHEEL: {
                SCROLL_DELTA->x += event.wheel.x;
                SCROLL_DELTA->y += event.wheel.y;
            }
                break;
            default:
                break;
        }
    }

    gamepadImpl->pollAxis(LEFT_THUMB_STICKS, RIGHT_THUMB_STICKS);
    gamepadImpl->pollControllers(gamepadButtonActions);
}

void Input::pollKeys() {
    for (std::pair<int, InputKey*> pair : keyActions)
        pair.second->frame();
}

void Input::pollButtons() {
    for (std::pair<int, InputButton*> pair : buttonActions)
        pair.second->frame();

    // float x,y;
    // SDL_MouseButtonFlags flags = SDL_GetMouseState(&x, &y);
    //
    //
}

void Input::pollAxis() {
    CommandContext ctx{};
    ctx.type = CommandType::AXIS_MOVE;
    float refx = CURSOR->x;
    float refy = CURSOR->y;
    SDL_GetMouseState(&CURSOR->x, &CURSOR->y);

    if (refx != CURSOR->x || refy != CURSOR->y) {
        ctx.axis = InputAxisType::CURSOR_POSITION;
        ctx.axisX = CURSOR->x;
        ctx.axisY = CURSOR->y;

        CURSOR->execute(ctx);
    }

    MOUSE_DELTA->x = CURSOR->x - refx;
    MOUSE_DELTA->y = CURSOR->y - refy;

    if (MOUSE_DELTA->x != 0 || MOUSE_DELTA->y != 0) {
        ctx.axis = InputAxisType::MOUSE_DELTA;
        ctx.axisX = MOUSE_DELTA->x;
        ctx.axisY = MOUSE_DELTA->y;

        MOUSE_DELTA->execute(ctx);
    }

    if (SCROLL_DELTA->x != 0 || SCROLL_DELTA->y != 0) {
        ctx.axis = InputAxisType::SCROLL;
        ctx.axisX = SCROLL_DELTA->x;
        ctx.axisY = SCROLL_DELTA->y;

        SCROLL_DELTA->execute(ctx);
    }

    SCROLL_DELTA->x = 0;
    SCROLL_DELTA->y = 0;
}

void Input::setPressed(InputGamepadButton *button, bool state) {
    button->pressed = state;
}

void Input::setXY(InputAxis *axis, float x, float y) {
    axis->x = x;
    axis->y = y;
}

void GamepadImplX::handleInputChanges(std::map<GamepadButton, InputGamepadButton*>& gamepadButtonActions, int changes, int state, int mask, uint32_t gamepadIndex) {
    if (changes & mask) {
        GamepadButton key = (GamepadButton)mask;
        bool pressed = state & mask;
        CommandContext ctx{};
        ctx.gamepadIndex = gamepadIndex;
        ctx.gamepadButton = key;
        ctx.type = pressed ? CommandType::GAMEPAD_BUTTON_PRESS : CommandType::GAMEPAD_BUTTON_RELEASE;
        this->input->getAnyPerformed().execute(ctx);

        if (gamepadButtonActions.contains(key)) {
            InputGamepadButton *button = gamepadButtonActions[key];
            setPressed(button, pressed);
            if (pressed)
                button->buttonPress(mask);
            else
                button->buttonRelease(mask);

            button->execute(ctx);
        }

        ctx.gamepadButton = GamepadButton::NONE;
        ctx.type = CommandType::NONE;
    }
}

void InputCommand::execute(const CommandContext & ctx) {
    for (CommandCallback& callback : onPerformed)
        callback.callback(ctx);
}

CommandCallbackId InputCommand::addListener(CommandCallbackFunc cb) {
    CommandCallback callback { ++callbackIndex, std::move(cb) };
    onPerformed.emplace_back(std::move(callback));
    return callbackIndex;
}

void InputCommand::removeListener(CommandCallbackId idx) {
    std::erase_if(onPerformed, [idx](const CommandCallback& callback) { return idx == callback.id; });
}

bool InputKey::isPressed() const {
    const bool* state = SDL_GetKeyboardState(nullptr);

    return state[key];
}

bool InputKey::pressedThisFrame() const {
    return framePress;
}

bool InputKey::releasedThisFrame() const {
    return frameRelease;
}

InputKey::InputKey(int k) : key{k} {

}

void InputKey::frame() {
    framePress = false;
    frameRelease = false;
}

void InputKey::keyPress(int _key) {
    if (this->key != _key)
        return;
    framePress = true;
    frameRelease = false;
}

void InputKey::keyRelease(int _key) {
    if (this->key != _key)
        return;
    framePress = false;
    frameRelease = true;
}

void InputKey::buttonPress(int) {
}

void InputKey::buttonRelease(int) {
}

bool InputButton::isPressed() const {
    float x,y;
    return SDL_GetMouseState(&x,&y) & SDL_BUTTON_MASK(button);
}

bool InputButton::pressedThisFrame() const {
    return framePress;
}

bool InputButton::releasedThisFrame() const {
    return frameRelease;
}

InputButton::InputButton(int button) : button{button} {

}

void InputButton::frame() {
    framePress = false;
    frameRelease = false;
}

void InputButton::keyPress(int) {
}

void InputButton::keyRelease(int) {
}

void InputButton::buttonPress(int _button) {
    if (this->button != _button)
        return;
    framePress = true;
    frameRelease = false;
}

void InputButton::buttonRelease(int _button) {
    if (this->button != _button)
        return;
    framePress = false;
    frameRelease = true;
}

float InputAxis::getX() const {
    return x;
}

float InputAxis::getY() const {
    return y;
}

InputAxis::InputAxis(InputAxisType axis) : axisType{axis} {

}

InputGamepadButton::InputGamepadButton(GamepadButton button) : button{button} {

}

bool InputGamepadButton::isPressed() const {
    return pressed;
}

bool InputGamepadButton::pressedThisFrame() const {
    return framePress;
}

bool InputGamepadButton::releasedThisFrame() const {
    return frameRelease;
}

void InputGamepadButton::frame() {
    framePress = false;
    frameRelease = false;
}

void InputGamepadButton::keyPress(int) {
}

void InputGamepadButton::keyRelease(int) {
}

void InputGamepadButton::buttonPress(int _button) {
    if (this->button != (GamepadButton)_button)
        return;
    framePress = true;
    frameRelease = false;
    pressed = true;
}

void InputGamepadButton::buttonRelease(int _button) {
    if (this->button != (GamepadButton)_button)
        return;
    framePress = false;
    frameRelease = true;
    pressed = false;
}

InputKey* Input::getKey(int key) {
    if (keyActions.count(key) == 0) {
        keyActions.emplace(key, new InputKey(key));
    }

    return keyActions[key];
}

InputButton* Input::getButton(int button) {
    if (buttonActions.count(button) == 0) {
        buttonActions.emplace(button, new InputButton(button));
    }

    return buttonActions[button];
}

InputGamepadButton* Input::getGamepadButton(GamepadButton button) {
    return getGamepadButton(0, button);
}

InputGamepadButton* Input::getGamepadButton(int gamepadIndex, GamepadButton button) {
    if (gamepadIndex < 0 || gamepadIndex >= MaxGamepads)
        gamepadIndex = 0;

    auto& playerActions = gamepadButtonActions[gamepadIndex];
    if (playerActions.count(button) == 0)
        playerActions.emplace(button, new InputGamepadButton(button));

    return playerActions[button];
}

InputAxis* Input::getMouseDelta() {
    return MOUSE_DELTA;
}

InputAxis* Input::getCursor() {
    return CURSOR;
}

InputAxis* Input::getScrollDelta() {
    return SCROLL_DELTA;
}

int Input::GetGamePad(int gamepadIndex) const {
    if (gamepadIndex < 0 || gamepadIndex >= MaxGamepads)
        gamepadIndex = 0;

    return gamepadImpl->GetGamepadButtons(gamepadIndex);
}

InputKey* Input::KEY(int key) {
    return Instance->getKey(key);
}

InputButton* Input::BUTTON(int button) {
    return Instance->getButton(button);
}

InputGamepadButton* Input::GAMEPAD_BUTTON(GamepadButton button) {
    return Instance->getGamepadButton(button);
}

InputGamepadButton* Input::GAMEPAD_BUTTON(int gamepadIndex, GamepadButton button) {
    return Instance->getGamepadButton(gamepadIndex, button);
}

InputAxis* Input::AXIS(InputAxisType type) {
    return AXIS(0, type);
}

InputAxis* Input::AXIS(int gamepadIndex, InputAxisType type) {
    if (gamepadIndex < 0 || gamepadIndex >= MaxGamepads)
        gamepadIndex = 0;

    if (type == InputAxisType::MOUSE_DELTA)
        return Instance->MOUSE_DELTA;
    else if (type == InputAxisType::SCROLL)
        return Instance->SCROLL_DELTA;
    else if (type == InputAxisType::GAMEPAD_LEFT)
        return Instance->LEFT_THUMB_STICKS[gamepadIndex];
    else if (type == InputAxisType::GAMEPAD_RIGHT)
        return Instance->RIGHT_THUMB_STICKS[gamepadIndex];
    else
        return Instance->CURSOR;
}
Input::GamepadImpl::GamepadImpl(Input *input) : input(input) {
}

#ifndef __EMSCRIPTEN__

void GamepadImplX::pollControllers(std::array<std::map<GamepadButton, InputGamepadButton*>, Input::MaxGamepads>& gamepadButtonActions) {
    for (DWORD player = 0; player < Input::MaxGamepads; ++player) {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));

        DWORD result = XInputGetState(player, &state);

        if (result != ERROR_SUCCESS) {
            gamepadButtons[player] = 0;
            continue;
        }

        int bState = 0;
        bState = state.Gamepad.wButtons;
        bState |= state.Gamepad.bLeftTrigger ? XINPUT_GAMEPAD_LEFT_TRIGGER : 0;
        bState |= state.Gamepad.bRightTrigger ? XINPUT_GAMEPAD_RIGHT_TRIGGER : 0;

        int changes = bState ^ gamepadButtons[player];

        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_DPAD_UP, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_DPAD_DOWN, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_DPAD_LEFT, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_DPAD_RIGHT, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_START, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_BACK, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_LEFT_THUMB, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_RIGHT_THUMB, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_LEFT_SHOULDER, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_RIGHT_SHOULDER, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_A, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_B, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_X, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_Y, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_LEFT_TRIGGER, player);
        handleInputChanges(gamepadButtonActions[player], changes, bState, XINPUT_GAMEPAD_RIGHT_TRIGGER, player);

        gamepadButtons[player] = bState;
    }
}

void GamepadImplX::pollAxis(std::array<InputAxis*, Input::MaxGamepads>& leftThumb, std::array<InputAxis*, Input::MaxGamepads>& rightThumb) {
    for (DWORD player = 0; player < Input::MaxGamepads; ++player) {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));

        DWORD result = XInputGetState(player, &state);

        if (result != ERROR_SUCCESS) {
            setXY(leftThumb[player], 0, 0);
            setXY(rightThumb[player], 0, 0);
            continue;
        }

        setXY(leftThumb[player], float(state.Gamepad.sThumbLX) / float(SHRT_MAX), float(state.Gamepad.sThumbLY) / float(SHRT_MAX));
        setXY(rightThumb[player], float(state.Gamepad.sThumbRX) / float(SHRT_MAX), float(state.Gamepad.sThumbRY) / float(SHRT_MAX));
    }
}
#endif
