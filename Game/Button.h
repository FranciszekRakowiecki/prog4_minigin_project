//
// Created by frane on 5/28/2026.
//

#ifndef PROG4MINIGINPROJECT_BUTTON_H
#define PROG4MINIGINPROJECT_BUTTON_H
#include "Component.h"
#include "Text.h"


class Button : public dae::Component {
    dae::Text m_Text{};
    std::function<void()> m_Callback{};
    bool m_IsSelected{false};
public:

    int GetFlags() override;

    void Start() override;

    void Update() override;

    void Render() override;

    void SetText(const std::string& text);

    glm::vec2 GetSize() const;
    void SetCallback(std::function<void()> callback);

    void SetSelected(bool state);

    void UpdateTexture();

    void Execute();
};


#endif //PROG4MINIGINPROJECT_BUTTON_H