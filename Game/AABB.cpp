//
// Created by frane on 5/28/2026.
//

#include "AABB.h"

bool AABB::contains(const glm::vec2 &point) {
    bool contains_x = x < point.x && point.x < x + z;
    bool contains_y = y < point.y && point.y < y + w;
    return contains_x && contains_y;
}

bool AABB::contains(const AABB &other) {
    bool contains_x = x < other.x && other.x + other.GetWidth() < x + GetWidth();
    bool contains_y = y < other.y && other.y + other.GetHeight() < y + GetHeight();
    return contains_x && contains_y;
}
