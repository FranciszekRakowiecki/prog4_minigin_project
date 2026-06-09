//
// Created by frane on 3/18/2026.
//

#include "Text.h"

#include <iostream>
#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "TextObject.h"
#include "Renderer.h"
#include "Font.h"
#include "Texture2D.h"

void dae::Text::SetText(const std::string &text) {
    if (text != m_text) {
        m_text = text;
        m_needsUpdate = true;
    }
}

void dae::Text::SetColor(const SDL_Color &color) {
    m_color = color;
    m_needsUpdate = true;
}

void dae::Text::SetFont(std::shared_ptr<Font> font) {
    m_font = font;
    m_needsUpdate = true;
}

dae::Text::Text(const std::string &text, std::shared_ptr<Font> font, const SDL_Color &color) : m_needsUpdate(true), m_text(text), m_color(color), m_font(std::move(font)), m_textTexture(nullptr) {
    UpdateTexture();
}

void dae::Text::UpdateTexture() {
    if (m_font == nullptr)
        return;
    if (m_needsUpdate)
    {
        SDL_Surface* surf;
        if (!m_text.empty()) {
            surf = TTF_RenderText_Blended(m_font->GetFont(), m_text.c_str(), m_text.length(), m_color);
            if (surf == nullptr)
            {
                std::string message = std::string("Render text failed: ") + SDL_GetError();
                std::cout << message << std::endl;
                throw std::runtime_error(message);
            }
        }
        else {
            surf = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_ABGR32);
        }
        auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
        if (texture == nullptr)
        {
            throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
        }
        SDL_DestroySurface(surf);
        m_textTexture = std::make_shared<Texture2D>(texture);
        m_needsUpdate = false;
    }
}

glm::vec2 dae::Text::GetTextureSize() const {
    if (m_textTexture != nullptr) {
        return m_textTexture->GetSize();
    }
    return {};
}
