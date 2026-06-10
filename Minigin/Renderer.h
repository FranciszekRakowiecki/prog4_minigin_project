#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include "Singleton.h"
#include "../Game/Shape.h"
#include "glm/vec4.hpp"

namespace dae {
	class Text;
}

namespace dae
{
	struct _Exercise2_Transform
	{
		float matrix[16] = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1 };
	};

	struct _Exercise2_GameObject {
		_Exercise2_Transform transform;
		int ID{1};
	};

	class Texture2D;
	/**
	 * Simple RAII wrapper for the SDL renderer
	 */
	class Renderer final : public Singleton<Renderer>
	{
		SDL_Renderer* m_renderer{};
		SDL_Window* m_window{};
		SDL_Color m_clearColor{};
	public:
		void Init(SDL_Window* window);
		void Render() const;
		void Destroy();

		void RenderTexture(const Texture2D& texture, float x, float y) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height) const;
		void RenderText(const Text& text, float x, float y) const;
		void RenderText(const Text& text, float x, float y, float width, float height) const;
		void RenderRect(float x, float y, float width, float height, const glm::vec4& color) const;
		void RenderRectWire(float x, float y, float width, float height, const glm::vec4& color) const;
		void RenderShape(const Shape::Buffer& worldBuffer, const Texture2D* texture = nullptr) const;

		SDL_Renderer* GetSDLRenderer() const;

		const SDL_Color& GetBackgroundColor() const { return m_clearColor; }
		void SetBackgroundColor(const SDL_Color& color) { m_clearColor = color; }
	};
}

