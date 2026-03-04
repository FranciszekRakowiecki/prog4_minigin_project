#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include "Singleton.h"

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

		std::vector<int> m_Exercise1_Timings;
		int m_Exercise1_Samples{10};

		std::vector<int> m_Exercise2_Timings;
		int m_Exercise2_Samples{10};
	public:
		void Init(SDL_Window* window);
		void Render() const;
		void Destroy();

		static void DrawExercise1();
		static void DrawExercise2();

		void RenderTexture(const Texture2D& texture, float x, float y) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height) const;

		SDL_Renderer* GetSDLRenderer() const;

		const SDL_Color& GetBackgroundColor() const { return m_clearColor; }
		void SetBackgroundColor(const SDL_Color& color) { m_clearColor = color; }
	};
}

