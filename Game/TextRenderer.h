//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_TEXTRENDERER_H
#define PROG4MINIGINPROJECT_TEXTRENDERER_H
#include "Component.h"
#include "Text.h"


class TextRenderer : public dae::Component {
    dae::Text m_Text;
public:
    int GetFlags() override;

    void Start() override;

    void Render() override;

    void SetText(const std::string& text);
    void SetFont(std::shared_ptr<dae::Font> font);
    glm::vec2 GetSize() const;
    void UpdateTexture();
    void SetColor(const SDL_Color& color);
};


#endif //PROG4MINIGINPROJECT_TEXTRENDERER_H