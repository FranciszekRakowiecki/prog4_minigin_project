//
// Created by frane on 6/9/2026.
//

#include "TankRenderer.h"

#include "GameObject.h"
#include "Renderer.h"

void TankRenderer::Start() {
    std::vector<SDL_Vertex> vertices;
    std::vector<int> indices {
        0, 1, 2, 0, 2, 3
    };

    AppendVertice(vertices, { 0.0f, 3.0f });
    AppendVertice(vertices, { 20.0f, 3.0f });
    AppendVertice(vertices, { 20.0f, -3.0f });
    AppendVertice(vertices, { 0.0f, -3.0f });

    m_Barrel.SetVertices(vertices);
    m_Barrel.SetIndices(indices);
    m_Barrel.SetColor({ 0.7f, 0.7f, 0.7f });
}

int TankRenderer::GetFlags() {
    return COMPONENT_HAS_RENDER;
}

void TankRenderer::Render() {
    const glm::vec3 position{GetParent()->transform.GetWorldPosition()};
    dae::Renderer::GetInstance().RenderRect(position.x - 10, position.y - 10, 20, 20, m_TankColor);
    dae::Renderer::GetInstance().RenderShape(m_Barrel.GetWorldBuffer(GetParent()->transform));
}

void TankRenderer::SetTankColor(const glm::vec4 &color) {
    m_TankColor = color;
}

const glm::vec4 & TankRenderer::GetTankColor() const {
    return m_TankColor;
}

void TankRenderer::AppendVertice(std::vector<SDL_Vertex> &vertices, const SDL_FPoint &point) {
    vertices.emplace_back(point, SDL_FColor{ 1.0f, 1.0f, 1.0f, 1.0f },SDL_FPoint{ 0.0f, 0.0f });
}
