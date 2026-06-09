//
// Created by frane on 3/18/2026.
//

#ifndef MINIGIN_TEXT_H
#define MINIGIN_TEXT_H
#include <memory>
#include <string>

#include "Font.h"
#include <SDL3/SDL.h>

#include "Renderer.h"
#include "Texture2D.h"


namespace dae {
    class Text {
    public:
        void SetText(const std::string& text);
        void SetColor(const SDL_Color& color);
        void SetFont(std::shared_ptr<Font> font);

        Text(const std::string& text = "", std::shared_ptr<Font> font = nullptr, const SDL_Color& color = { 255, 255, 255, 255 });

        Text(const Text& other) = delete;
        Text(Text&& other) = delete;
        Text& operator=(const Text& other) = delete;
        Text& operator=(Text&& other) = delete;

        void UpdateTexture();

        glm::vec2 GetTextureSize() const;

    private:
        bool m_needsUpdate{};
        std::string m_text{};
        SDL_Color m_color{ 255, 255, 255, 255 };
        std::shared_ptr<Font> m_font{};
        std::shared_ptr<Texture2D> m_textTexture{};

        friend void dae::Renderer::RenderText(const Text &text, float x, float y) const;
        friend void dae::Renderer::RenderText(const Text &text, float x, float y, float w, float h) const;
    };
}


#endif //MINIGIN_TEXT_H