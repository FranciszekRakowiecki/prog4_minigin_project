//
// Created by frane on 6/9/2026.
//

#include "LevelRenderer.h"

#include "GameObject.h"
#include "Minigin.h"
#include "Renderer.h"

int LevelRenderer::GetFlags() {
    return COMPONENT_HAS_RENDER;
}

void LevelRenderer::Render() {
    if (m_LevelData) {
        const glm::vec3& position{GetParent()->transform.GetWorldPosition()};
        for (int x = 0; x < m_LevelData->width; ++x) {
            for (int y = 0; y < m_LevelData->height; ++y) {
                if (m_LevelData->IsWall(x,y)) {
                    dae::Renderer::GetInstance().RenderRect(position.x + x * m_TileSize, position.y + y * m_TileSize, m_TileSize, m_TileSize, { 0.2f, 1.0f, 0.2f, 1.0f });
                }
            }
        }
    }
}

void LevelRenderer::SetLevelData(LevelData *levelData) {
    m_LevelData = levelData;
    if (m_LevelData == nullptr)
        return;
    glm::vec2 windowSize{ dae::Minigin::GetInstance().GetWindowSize() };

    // Leave some space for high score
    m_TileSize = (windowSize.y - 100.0f) / m_LevelData->height;
}

void LevelRenderer::Start() {

}

float LevelRenderer::GetResolution() const {
    if (m_LevelData) {
        return m_LevelData->height * m_TileSize;
    }
    return 0.0f;
}
