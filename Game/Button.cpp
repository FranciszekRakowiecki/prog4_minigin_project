//
// Created by frane on 5/28/2026.
//

#include "Button.h"

#include "Game.h"
#include "GameObject.h"

int Button::GetFlags() {
    return COMPONENT_HAS_RENDER | COMPONENT_HAS_UPDATE;
}

void Button::Start() {
    m_Text.SetFont(Game::GetInstance().GetGameFont());
}

void Button::Update() {

}

void Button::Render() {
    glm::vec3 position = GetParent()->transform.GetWorldPosition();
    dae::Renderer::GetInstance().RenderText(m_Text, position.x, position.y, m_Size.x, m_Size.y);
}

void Button::SetText(const std::string &text) {
    m_Text.SetText(text);
}

void Button::SetSize(float width, float height) {
    m_Size.x = width;
    m_Size.y = height;
}

const glm::vec2 & Button::GetSize() const {
    return m_Size;
}