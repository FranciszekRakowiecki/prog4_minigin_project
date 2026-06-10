//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_SHAPE_H
#define PROG4MINIGINPROJECT_SHAPE_H
#include <vector>

#include <SDL3/SDL_render.h>

#include "Transform.h"
#include "glm/vec4.hpp"


class Shape {
public:
    struct Buffer {
        std::vector<SDL_Vertex> vertices{};
        std::vector<int> indices{};
        glm::vec3 color{1.0f, 1.0f, 1.0f };
    };

    void SetVertices(const std::vector<SDL_Vertex>& vertices);
    void SetIndices(const std::vector<int>& indices);
    void SetColor(const glm::vec3& color);

    const Buffer& GetObjectSpaceBuffer() const;
    const Buffer& GetWorldBuffer(dae::Transform& transform);

private:
    Buffer m_ObjectSpace{};
    Buffer m_WorldSpace{};
};


#endif //PROG4MINIGINPROJECT_SHAPE_H