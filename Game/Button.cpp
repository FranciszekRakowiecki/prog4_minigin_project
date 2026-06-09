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
    m_Text.SetText("");
    m_Text.SetColor({ 255, 255, 255, 255 });
    m_Text.UpdateTexture();
}

void Button::Update() {

}

void Button::Render() {
    glm::vec3 position = GetParent()->transform.GetWorldPosition();
    m_Text.UpdateTexture();
    if (m_IsSelected) {
        glm::vec2 size{m_Text.GetTextureSize()};
        dae::Renderer::GetInstance().RenderRectWire(position.x, position.y, size.x, size.y, { 0.3f, 0.3f, 1.0f, 1.0f });
    }
    dae::Renderer::GetInstance().RenderText(m_Text, position.x, position.y);
}

void Button::SetText(const std::string &text) {
    m_Text.SetText(text);
}

glm::vec2 Button::GetSize() const {
    return m_Text.GetTextureSize();
}

void Button::SetCallback(std::function<void()> callback) {
    this->m_Callback = callback;
}

void Button::SetSelected(bool state) {
    m_IsSelected = state;
}

void Button::UpdateTexture() {
    m_Text.UpdateTexture();
}

void Button::Execute() {
    if (m_Callback) {
        m_Callback();
    }
}
