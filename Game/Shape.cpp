//
// Created by frane on 6/9/2026.
//

#include "Shape.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Shape::SetVertices(const std::vector<SDL_Vertex> &vertices) {
    m_ObjectSpace.vertices = vertices;
}

void Shape::SetIndices(const std::vector<int> &indices) {
    m_ObjectSpace.indices = indices;
    m_WorldSpace.indices = m_ObjectSpace.indices;
}

void Shape::SetColor(const glm::vec3 &color) {
    m_ObjectSpace.color = color;
    m_WorldSpace.color = m_ObjectSpace.color;
}

const Shape::Buffer & Shape::GetObjectSpaceBuffer() const {
    return m_ObjectSpace;
}

const Shape::Buffer & Shape::GetWorldBuffer(dae::Transform &transform) {
    m_WorldSpace.vertices.resize(m_ObjectSpace.vertices.size());
    glm::mat4 m4{glm::identity<glm::mat4>()};
    m4 = glm::translate(m4, transform.GetWorldPosition());
    m4 = glm::rotate(m4, transform.GetRotation(), { 0, 0, 1});
    for (int index = 0; index < m_ObjectSpace.vertices.size(); ++index) {
        SDL_Vertex& vertex = m_ObjectSpace.vertices[index];
        SDL_Vertex& destination = m_WorldSpace.vertices[index];
        glm::vec4 worldVertex = m4 * glm::vec4{ vertex.position.x, vertex.position.y, 0.0f, 1.0f };
        destination.position.x = worldVertex.x;
        destination.position.y = worldVertex.y;
        destination.color = { m_ObjectSpace.color.r, m_ObjectSpace.color.g, m_ObjectSpace.color.b, 1.0f };
        destination.tex_coord = vertex.tex_coord;
    }

    return m_WorldSpace;
}
