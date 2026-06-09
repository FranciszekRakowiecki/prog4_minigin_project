//
// Created by frane on 6/9/2026.
//

#include "TextRenderer.h"

#include "Game.h"
#include "GameObject.h"

int TextRenderer::GetFlags() {
    return COMPONENT_HAS_RENDER;
}

void TextRenderer::Start() {
    m_Text.SetFont(Game::GetInstance().GetGameFont());
    m_Text.SetText("");
    m_Text.SetColor({ 255, 255, 255, 255 });
    m_Text.UpdateTexture();
}

void TextRenderer::Render() {
    UpdateTexture();
    const glm::vec3& position{GetParent()->transform.GetWorldPosition()};
    dae::Renderer::GetInstance().RenderText(m_Text, position.x, position.y);
}

void TextRenderer::SetText(const std::string &text) {
    m_Text.SetText(text);
}

void TextRenderer::SetFont(std::shared_ptr<dae::Font> font) {
    m_Text.SetFont(font);
}

glm::vec2 TextRenderer::GetSize() const {
    return m_Text.GetTextureSize();
}

void TextRenderer::UpdateTexture() {
    m_Text.UpdateTexture();
}

void TextRenderer::SetColor(const SDL_Color &color) {
    m_Text.SetColor(color);
}
