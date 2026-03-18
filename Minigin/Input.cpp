//
// Created by frane on 3/11/2026.
//

#include "Input.h"

#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Xinput.h>

#include "Minigin.h"
#include <SDL3/SDL.h>

using namespace dae;

Input* Input::Instance{nullptr};

Input::Input(Minigin* engine, SDL_Window* window) :
    engine(engine),
    MOUSE_DELTA{new InputAxis(MOUSE)},
    CURSOR{new InputAxis(CURSOR_POSITION)},
    SCROLL_DELTA{new InputAxis(SCROLL)},
    LEFT_THUMB_STICK(new InputAxis(GAMEPAD_LEFT)),
    RIGHT_THUMB_STICK(new InputAxis(GAMEPAD_RIGHT)),
    window(window),
    gamepadButtons(0)
{
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

    for (std::pair<int, InputGamepadButton*> pair : gamepadButtonActions)
        delete pair.second;

    keyActions.clear();
    buttonActions.clear();
    gamepadButtonActions.clear();
}

void Input::pollEvents() {
    pollControllers();
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
            default:
                break;
        }
    }
}

void Input::pollControllers() {
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    DWORD result = XInputGetState(0, &state);

    if (result != ERROR_SUCCESS) {
        return;
    }

    LEFT_THUMB_STICK->x = float(state.Gamepad.sThumbLX) / float(SHRT_MAX);
    LEFT_THUMB_STICK->y = float(state.Gamepad.sThumbLY) / float(SHRT_MAX);

    RIGHT_THUMB_STICK->x = float(state.Gamepad.sThumbRX) / float(SHRT_MAX);
    RIGHT_THUMB_STICK->y = float(state.Gamepad.sThumbRY) / float(SHRT_MAX);

    int bState = 0;
    bState = state.Gamepad.wButtons;
    bState |= state.Gamepad.bLeftTrigger ? XINPUT_GAMEPAD_LEFT_TRIGGER : 0;
    bState |= state.Gamepad.bRightTrigger ? XINPUT_GAMEPAD_RIGHT_TRIGGER : 0;

    int changes = bState ^ gamepadButtons;

    handleInputChanges(changes, bState, XINPUT_GAMEPAD_DPAD_UP);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_DPAD_DOWN);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_DPAD_LEFT);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_DPAD_RIGHT);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_START);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_BACK);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_LEFT_THUMB);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_RIGHT_THUMB);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_LEFT_SHOULDER);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_RIGHT_SHOULDER);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_A);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_B);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_X);
    handleInputChanges(changes, bState, XINPUT_GAMEPAD_Y);

    gamepadButtons = bState;
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

void Input::handleInputChanges(int changes, int state, int mask) {
    if (changes & mask)
        if (gamepadButtonActions.contains(mask)) {
            bool pressed = state & mask;
            InputGamepadButton* button = gamepadButtonActions[mask];
            button->pressed = pressed;
            if (pressed)
                button->buttonPress(mask);
            else
                button->buttonRelease(mask);
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

InputGamepadButton::InputGamepadButton(int button) : button{button} {

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
    if (this->button != _button)
        return;
    framePress = true;
    frameRelease = false;
    pressed = true;
}

void InputGamepadButton::buttonRelease(int _button) {
    if (this->button != _button)
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

const InputGamepadButton* Input::getGamepadButton(int button) {
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

const InputKey* Input::KEY(int key) {
    return Instance->getKey(key);
}

const InputButton* Input::BUTTON(int button) {
    return Instance->getButton(button);
}

const InputGamepadButton* Input::GAMEPAD_BUTTON(int button) {
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
