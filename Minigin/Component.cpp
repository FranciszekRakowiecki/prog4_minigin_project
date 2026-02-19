//
// Created by frane on 2/18/2026.
//

#include "Component.h"

#include <iostream>

#include "GameTime.h"
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