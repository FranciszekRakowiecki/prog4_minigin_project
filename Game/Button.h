//
// Created by frane on 5/28/2026.
//

#ifndef PROG4MINIGINPROJECT_BUTTON_H
#define PROG4MINIGINPROJECT_BUTTON_H
#include "Component.h"
#include "Text.h"


class Button : public dae::Component {
    dae::Text m_Text;
    glm::vec2 m_Size;
public:

    int GetFlags() override;

    void Start() override;

    void Update() override;

    void Render() override;

    void SetText(const std::string& text);

    void SetSize(float width, float height);
    const glm::vec2& GetSize() const;
};


#endif //PROG4MINIGINPROJECT_BUTTON_H