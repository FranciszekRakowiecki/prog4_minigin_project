//
// Created by frane on 2/18/2026.
//

#include "Component.h"

#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Xinput.h>

#include "GameTime.h"
#include "Input.h"
#include "Minigin.h"
#include "ServiceLocator.h"
#include "SoundLibrary.h"
#include "TextObject.h"

void dae::Component::Destroy() {
    m_IsDisposed = true;
}

dae::GameObject * dae::Component::GetParent() const {
    if (m_IsDisposed)
        return nullptr;
    return m_Parent;
}

int dae::Component::GetFlags() {
    return 0;
}

void dae::Component::Start() {
}

void dae::Component::Update() {
}

void dae::Component::Render() {
}

int dae::ExampleUpdateComponent::GetFlags() {
    return COMPONENT_HAS_UPDATE;
}

void dae::ExampleUpdateComponent::Update() {
    TextObject* parent = dynamic_cast<TextObject*>(GetParent());
    if (parent) {
        float clampedFPS = roundf(1.0f / GameTime::GetInstance().GetDeltaTime() * 10.0f) / 10.0f;
        parent->SetText("FPS: " + std::to_string(clampedFPS));
    }
}

dae::ExampleRotator::ExampleRotator() {

}

int dae::ExampleRotator::GetFlags() {
    return COMPONENT_HAS_UPDATE;
}

void dae::ExampleRotator::Update() {
    Transform& transform = GetParent()->transform;

    transform.SetRotation(transform.GetRotation() + GameTime::GetInstance().GetDeltaTime() * rotatorSpeed);

    glm::vec3 pos = glm::vec3{cosf(transform.GetRotation()) * 100.0f, sinf(transform.GetRotation()) * 100.0f, 0.0f};

    transform.SetLocalPosition(pos + offset);
}

void dae::ExampleRotator::Start() {
    offset = GetParent()->transform.GetPosition();
}

dae::ExampleMovementKeyboard::ExampleMovementKeyboard() :
forward{Input::KEY(SDL_SCANCODE_W)},
back{Input::KEY(SDL_SCANCODE_S)},
left{Input::KEY(SDL_SCANCODE_A)},
right{Input::KEY(SDL_SCANCODE_D)},
playSound(Input::KEY(SDL_SCANCODE_R))
{
}

void dae::ExampleMovementKeyboard::Update() {
    const float deltaTime = GameTime::GetInstance().GetDeltaTime();

    const float x = (right->isPressed() ? 1.0f : 0.0f) + (left->isPressed() ? -1.0f : 0.0f);
    const float y = (forward->isPressed() ? 1.0f : 0.0f) + (back->isPressed() ? -1.0f : 0.0f);

    GameObject* parent = GetParent();

    parent->transform.SetWorldPosition(parent->transform.GetWorldPosition() + glm::vec3{ x, y, 0.0f } * deltaTime * 20.0f);

    if (playSound->pressedThisFrame()) {
        ServiceLocator::GetInstance().getSoundSystem().playSound(SoundLibrary::GetInstance().Beep, 1.0f);
        ServiceLocator::GetInstance().getSoundSystem().playSound(SoundLibrary::GetInstance().Boop, 1.0f);
    }
}

int dae::ExampleMovementKeyboard::GetFlags() {
    return COMPONENT_HAS_UPDATE;
}

dae::ExampleMovementDPAD::ExampleMovementDPAD() :
forward{Input::GAMEPAD_BUTTON(GamepadButton::DPAD_UP)},
back{Input::GAMEPAD_BUTTON(GamepadButton::DPAD_DOWN)},
left{Input::GAMEPAD_BUTTON(GamepadButton::DPAD_LEFT)},
right{Input::GAMEPAD_BUTTON(GamepadButton::DPAD_RIGHT)}
{

}

void dae::ExampleMovementDPAD::Update() {
    const float deltaTime = GameTime::GetInstance().GetDeltaTime();

    const float x = (right->isPressed() ? 1.0f : 0.0f) + (left->isPressed() ? -1.0f : 0.0f);
    const float y = (forward->isPressed() ? 1.0f : 0.0f) + (back->isPressed() ? -1.0f : 0.0f);

    GameObject* parent = GetParent();

    parent->transform.SetWorldPosition(parent->transform.GetWorldPosition() + glm::vec3{ x, y, 0.0f } * deltaTime * 20.0f);
}

int dae::ExampleMovementDPAD::GetFlags() {
    return COMPONENT_HAS_UPDATE;
}
