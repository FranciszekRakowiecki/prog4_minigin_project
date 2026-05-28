//
// Created by frane on 5/28/2026.
//

#ifndef PROG4MINIGINPROJECT_AABB_H
#define PROG4MINIGINPROJECT_AABB_H
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>


class AABB : public glm::vec4 {
    bool contains(const glm::vec2& point);
    bool contains(const AABB& other);

    float GetX() const { return x; }
    float GetY() const { return y; }

    float GetWidth() const { return z; }
    float GetHeight() const { return w; }
};


#endif //PROG4MINIGINPROJECT_AABB_H