//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_TANKRENDERER_H
#define PROG4MINIGINPROJECT_TANKRENDERER_H
#include "Component.h"
#include "Shape.h"


class TankRenderer : public dae::Component {
public:
    void Start() override;

    int GetFlags() override;

    void Render() override;

    void SetTankColor(const glm::vec4& color);
    const glm::vec4& GetTankColor() const;

private:
    Shape m_Barrel{};
    glm::vec4 m_TankColor{1.0f, 0.0f, 0.0f, 1.0f};

    void AppendVertice(std::vector<SDL_Vertex>& vertices, const SDL_FPoint& point);
};


#endif //PROG4MINIGINPROJECT_TANKRENDERER_H