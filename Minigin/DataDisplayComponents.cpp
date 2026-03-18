//
// Created by frane on 3/18/2026.
//

#include "DataDisplayComponents.h"

#include "GameObject.h"
#include "Renderer.h"
#include "Text.h"

void LivesScoreRenderer::setPlayers(dae::GameObject *playerOne, dae::GameObject *playerTwo) {
    m_playerOne = playerOne;
    m_playerTwo = playerTwo;
}

void LivesScoreRenderer::setFont(dae::Font *font) {
    m_font = font;
}

void LivesScoreRenderer::Render() {
    dae::Transform& transform = GetParent()->transform;
    glm::vec3 position = transform.GetWorldPosition();

    dae::Renderer::GetInstance().RenderText(*m_livesOne, position.x, position.y);
    dae::Renderer::GetInstance().RenderText(*m_scoreOne, position.x, position.y + 36);
    dae::Renderer::GetInstance().RenderText(*m_livesTwo, position.x, position.y + 36 * 2);
    dae::Renderer::GetInstance().RenderText(*m_scoreTwo, position.x, position.y + 36 * 3);
}

void LivesScoreRenderer::Update() {
    m_livesOne->updateTexture();
    m_livesTwo->updateTexture();
    m_scoreOne->updateTexture();
    m_scoreTwo->updateTexture();
}

int LivesScoreRenderer::GetFlags() {
    return COMPONENT_HAS_RENDER | COMPONENT_HAS_UPDATE;
}
