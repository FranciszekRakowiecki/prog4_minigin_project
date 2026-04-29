//
// Created by frane on 3/11/2026.
//

#include "Input.h"

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

    int gamepadButtons{};
    virtual void pollControllers(std::map<GamepadButton, InputGamepadButton*>&) {};
    virtual void pollAxis(InputAxis*, InputAxis*) {};
    virtual void onGamepadDown(SDL_Event&) {};
    virtual void onGamepadUp(SDL_Event&) {};
    virtual void onJoystickMove(SDL_Event&) {};
    virtual int GetGamepadButtons() { return gamepadButtons; }
    virtual void setGamepad(SDL_Gamepad*) {};
    virtual void removeGamepad(SDL_Gamepad*) {};
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

    void onGamepadDown(SDL_Event &event) override;
    void onGamepadUp(SDL_Event &event) override;
    void onJoystickMove(SDL_Event &event) override;
    void setGamepad(SDL_Gamepad *) override;
    void removeGamepad(SDL_Gamepad *) override;
    void frame() override;
    void pollAxis(InputAxis *leftThumb, InputAxis *rightThumb) override;

    int GPBasMask(SDL_Gamepad* pad, GamepadButton mask, SDL_GamepadButton button);
    void handleInputChanges(std::map<GamepadButton, InputGamepadButton*>& gamepadButtons, int changes, int state, int mask);
};

void GamepadImplSDL::onGamepadDown(SDL_Event &event) {
    SDL_Gamepad* gamepad = SDL_GetGamepadFromID(event.gdevice.which);

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

    int changes = state ^ gamepadButtons;

    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::DPAD_UP);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::DPAD_DOWN);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::DPAD_LEFT);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::DPAD_RIGHT);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::A);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::B);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::X);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::Y);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::START);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::BACK);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::LEFT_SHOULDER);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::RIGHT_SHOULDER);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::LEFT_THUMB);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::RIGHT_THUMB);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::LEFT_TRIGGER);
    handleInputChanges(input->gamepadButtonActions, changes, state, (int)GamepadButton::RIGHT_TRIGGER);

    gamepadButtons = state;
}

void GamepadImplSDL::onGamepadUp(SDL_Event &event) {
    GamepadImpl::onGamepadUp(event);
}

void GamepadImplSDL::onJoystickMove(SDL_Event &event) {
    GamepadImpl::onJoystickMove(event);
}

void GamepadImplSDL::setGamepad(SDL_Gamepad *sdl_gamepad) {
    GamepadImpl::setGamepad(sdl_gamepad);
}

void GamepadImplSDL::removeGamepad(SDL_Gamepad *sdl_gamepad) {
    GamepadImpl::removeGamepad(sdl_gamepad);
}

void GamepadImplSDL::frame() {
    GamepadImpl::frame();
}

void GamepadImplSDL::pollAxis(InputAxis *leftThumb, InputAxis *rightThumb) {
    GamepadImpl::pollAxis(leftThumb, rightThumb);
}

int GamepadImplSDL::GPBasMask(SDL_Gamepad *pad, GamepadButton mask, SDL_GamepadButton button) {
    if (SDL_GetGamepadButton(pad, button))
        return (int)mask;
    return 0;
}

void GamepadImplSDL::handleInputChanges(std::map<GamepadButton, InputGamepadButton *> &gamepadButtons, int changes,
    int state, int mask) {
    if (changes & mask) {
        GamepadButton key = (GamepadButton)mask;
        if (gamepadButtons.contains(key)) {
            bool pressed = state & mask;
            InputGamepadButton *button = gamepadButtons[key];
            setPressed(button, pressed);
            if (pressed)
                button->buttonPress(mask);
            else
                button->buttonRelease(mask);
        }
    }
}

class GamepadImplX : public Input::GamepadImpl {
public:
    explicit GamepadImplX(Input *input)
        : GamepadImpl(input) {
    }
    void pollControllers(std::map<GamepadButton, InputGamepadButton*>& gamepadButtonActions) override;
    void pollAxis(InputAxis *leftThumb, InputAxis *rightThumb) override;

    void handleInputChanges(std::map<GamepadButton, InputGamepadButton*>& gamepadButtons, int changes, int state, int mask);
};

Input::Input(Minigin* engine, SDL_Window* window) :
    engine(engine),
    MOUSE_DELTA{new InputAxis(MOUSE)},
    CURSOR{new InputAxis(CURSOR_POSITION)},
    SCROLL_DELTA{new InputAxis(SCROLL)},
    LEFT_THUMB_STICK(new InputAxis(GAMEPAD_LEFT)),
    RIGHT_THUMB_STICK(new InputAxis(GAMEPAD_RIGHT)),
    window(window),
gamepadImpl(nullptr)
{
    #ifdef __EMSCRIPTEN__
    gamepadImpl = std::make_unique<GamepadImplSDL>(this)
    #else
    gamepadImpl = std::make_unique<GamepadImplX>(this);
    #endif
    Instance = this;
}

Input::~Input() {
    delete MOUSE_DELTA;
    delete SCROLL_DELTA;
    delete CURSOR;
    delete RIGHT_THUMB_STICK;
    delete LEFT_THUMB_STICK;

    for (std::pair<int, InputKey*> pair : keyActions)
        delete pair.second;

    for (std::pair<int, InputButton*> pair : buttonActions)
        delete pair.second;

    for (std::pair<GamepadButton, InputGamepadButton*> pair : gamepadButtonActions)
        delete pair.second;

    keyActions.clear();
    buttonActions.clear();
    gamepadButtonActions.clear();
}

void Input::pollEvents() {
    gamepadImpl->frame();
    pollKeys();
    pollButtons();
    pollAxis();

    SDL_PumpEvents();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                engine->Stop();
                return;
            case SDL_EVENT_KEY_DOWN:
                if (keyActions.contains(event.key.scancode)) {
                    InputKey* key = keyActions[event.key.scancode];
                    key->keyPress(event.key.scancode);
                }
                break;
            case SDL_EVENT_KEY_UP:
                if (keyActions.contains(event.key.scancode)) {
                    InputKey* key = keyActions[event.key.scancode];
                    key->keyRelease(event.key.scancode);
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (buttonActions.contains(event.button.button)) {
                    InputButton* button = buttonActions[event.button.button];
                    button->buttonPress(event.button.button);
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (buttonActions.contains(event.button.button)) {
                    InputButton* button = buttonActions[event.button.button];
                    button->buttonRelease(event.button.button);
                }
                break;
            case SDL_EVENT_GAMEPAD_ADDED: {
                SDL_Gamepad *gamepad = SDL_GetGamepadFromID(event.gdevice.which);
                gamepadImpl->setGamepad(gamepad);
            }
                break;
            case SDL_EVENT_GAMEPAD_REMOVED: {
                SDL_Gamepad *gamepad = SDL_GetGamepadFromID(event.gdevice.which);
                gamepad = SDL_GetGamepadFromID(event.gdevice.which);
                gamepadImpl->removeGamepad(gamepad);
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
            default:
                break;
        }
    }

    gamepadImpl->pollAxis(LEFT_THUMB_STICK, RIGHT_THUMB_STICK);
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
    float refx = CURSOR->x;
    float refy = CURSOR->y;
    SDL_GetMouseState(&CURSOR->x, &CURSOR->y);

    MOUSE_DELTA->x = CURSOR->x - refx;
    MOUSE_DELTA->y = CURSOR->y - refy;

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

void GamepadImplX::handleInputChanges(std::map<GamepadButton, InputGamepadButton*>& gamepadButtons, int changes, int state, int mask) {
    if (changes & mask) {
        GamepadButton key = (GamepadButton)mask;
        if (gamepadButtons.contains(key)) {
            bool pressed = state & mask;
            InputGamepadButton *button = gamepadButtons[key];
            setPressed(button, pressed);
            if (pressed)
                button->buttonPress(mask);
            else
                button->buttonRelease(mask);
        }
    }
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

const InputKey* Input::getKey(int key) {
    if (keyActions.count(key) == 0) {
        keyActions.emplace(key, new InputKey(key));
    }

    return keyActions[key];
}

const InputButton* Input::getButton(int button) {
    if (buttonActions.count(button) == 0) {
        buttonActions.emplace(button, new InputButton(button));
    }

    return buttonActions[button];
}

const InputGamepadButton* Input::getGamepadButton(GamepadButton button) {
    if (gamepadButtonActions.count(button) == 0) {
        gamepadButtonActions.emplace(button, new InputGamepadButton(button));
    }

    return gamepadButtonActions[button];
}

const InputAxis* Input::getMouseDelta() {
    return MOUSE_DELTA;
}

const InputAxis* Input::getCursor() {
    return CURSOR;
}

const InputAxis* Input::getScrollDelta() {
    return SCROLL_DELTA;
}

int Input::GetGamePad() const {
    return gamepadImpl->GetGamepadButtons();
}

const InputKey* Input::KEY(int key) {
    return Instance->getKey(key);
}

const InputButton* Input::BUTTON(int button) {
    return Instance->getButton(button);
}

const InputGamepadButton* Input::GAMEPAD_BUTTON(GamepadButton button) {
    return Instance->getGamepadButton(button);
}

const InputAxis* Input::AXIS(InputAxisType type) {
    if (type == MOUSE)
        return Instance->MOUSE_DELTA;
    else if (type == SCROLL)
        return Instance->SCROLL_DELTA;
    else if (type == GAMEPAD_LEFT)
        return Instance->LEFT_THUMB_STICK;
    else if (type == GAMEPAD_RIGHT)
        return Instance->RIGHT_THUMB_STICK;
    else
        return Instance->CURSOR;
}
Input::GamepadImpl::GamepadImpl(Input *input) : input(input) {
}

#ifndef __EMSCRIPTEN__

void GamepadImplX::pollControllers(std::map<GamepadButton, InputGamepadButton*>& gamepadButtonActions) {
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    DWORD result = XInputGetState(0, &state);

    if (result != ERROR_SUCCESS) {
        return;
    }

    int bState = 0;
    bState = state.Gamepad.wButtons;
    bState |= state.Gamepad.bLeftTrigger ? XINPUT_GAMEPAD_LEFT_TRIGGER : 0;
    bState |= state.Gamepad.bRightTrigger ? XINPUT_GAMEPAD_RIGHT_TRIGGER : 0;

    int changes = bState ^ gamepadButtons;

    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_DPAD_UP);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_DPAD_DOWN);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_DPAD_LEFT);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_DPAD_RIGHT);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_START);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_BACK);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_LEFT_THUMB);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_RIGHT_THUMB);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_LEFT_SHOULDER);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_RIGHT_SHOULDER);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_A);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_B);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_X);
    handleInputChanges(gamepadButtonActions, changes, bState, XINPUT_GAMEPAD_Y);

    gamepadButtons = bState;
}

void GamepadImplX::pollAxis(InputAxis *leftThumb, InputAxis *rightThumb) {
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    DWORD result = XInputGetState(0, &state);

    if (result != ERROR_SUCCESS) {
        return;
    }

    setXY(leftThumb, float(state.Gamepad.sThumbLX) / float(SHRT_MAX), float(state.Gamepad.sThumbLY) / float(SHRT_MAX));

    // leftThumb->x = float(state.Gamepad.sThumbLX) / float(SHRT_MAX);
    // leftThumb->y = float(state.Gamepad.sThumbLY) / float(SHRT_MAX);

    setXY(rightThumb, float(state.Gamepad.sThumbRX) / float(SHRT_MAX), float(state.Gamepad.sThumbRY) / float(SHRT_MAX));

    // rightThumb->x = float(state.Gamepad.sThumbRX) / float(SHRT_MAX);
    // rightThumb->y = float(state.Gamepad.sThumbRY) / float(SHRT_MAX);
}
#endif
