#include <stdexcept>
#include <cstring>
#include <iostream>
#include "Renderer.h"

#include <chrono>

#include <implot.h>
#include "SceneManager.h"
#include "Texture2D.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

void dae::Renderer::Init(SDL_Window* window)
{
	m_window = window;

	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

#if defined(__EMSCRIPTEN__)
	m_renderer = SDL_CreateRenderer(window, nullptr);
#else
	m_renderer = SDL_CreateRenderer(window, nullptr);
#endif

	if (m_renderer == nullptr)
	{
		std::cout << "Failed to create the renderer: " << SDL_GetError() << "\n";
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImPlot::CreateContext();

	#if __EMSCRIPTEN__
		io.IniFilename = NULL;
	#endif

	ImGui_ImplSDL3_InitForSDLRenderer(m_window, m_renderer);
	ImGui_ImplSDLRenderer3_Init(m_renderer);
}

void dae::Renderer::Render() const
{
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// ImGui::ShowDemoWindow();

	DrawExercise1();
	DrawExercise2();

	ImGui::Render();

	const auto& color = GetBackgroundColor();
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_renderer);

	SceneManager::GetInstance().Render();

	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);

	SDL_RenderPresent(m_renderer);
}

void dae::Renderer::Destroy()
{
	ImPlot::DestroyContext();
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	if (m_renderer != nullptr)
	{
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}
}

void dae::Renderer::DrawExercise1() {
	if (ImGui::Begin("Exercise 1")) {
		ImGui::InputInt("Samples", &GetInstance().m_Exercise1_Samples);
		GetInstance().m_Exercise1_Samples = std::max(GetInstance().m_Exercise1_Samples, 1);

		if (ImGui::Button("Thrash the cache")) {
			GetInstance().m_Exercise1_Timings.clear();
			const int length = 4194304;
			int* arr = new int[length];

			int count = 0;

			{
				int step = 1;
				while (step <= 1024) {
					count++;
					step *= 2;
				}
			}

			std::cout << count << "\n";

			GetInstance().m_Exercise1_Timings.resize(count);

			for (int index = 0; index < GetInstance().m_Exercise1_Samples; ++index) {
				int step = 1;
				int c = 0;
				while (step <= 1024) {
					const auto start = std::chrono::high_resolution_clock::now();
					for (int i = 0; i < length; i += step) arr[i] *= 2;
					step *= 2;
					const auto end = std::chrono::high_resolution_clock::now();
					const auto total = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
					std::cout << step << " " << total << "\n";
					GetInstance().m_Exercise1_Timings[c] += (int)total;
					c++;
				}
			}
			for (int index = 0; index < (int)GetInstance().m_Exercise1_Timings.size(); ++index) {
				GetInstance().m_Exercise1_Timings[index] /= GetInstance().m_Exercise1_Samples;
			}

			delete[] arr;
		}
		if (!GetInstance().m_Exercise1_Timings.empty()) {
			ImPlot::BeginPlot("##timings_plot");
			ImPlot::PlotLine("##timings", GetInstance().m_Exercise1_Timings.data(), (int)GetInstance().m_Exercise1_Timings.size());
			ImPlot::EndPlot();
		}
	}
	ImGui::End();
}

void dae::Renderer::DrawExercise2() {
	if (ImGui::Begin("Exercise 2")) {
		ImGui::InputInt("Samples", &GetInstance().m_Exercise2_Samples);
		GetInstance().m_Exercise2_Samples = std::max(GetInstance().m_Exercise2_Samples, 1);

		if (ImGui::Button("Thrash the cache with gameobject3D")) {
			GetInstance().m_Exercise2_Timings.clear();
			const int length = 4194304;
			_Exercise2_GameObject* arr = new _Exercise2_GameObject[length];
			int count = 0;

			{
				int step = 1;
				while (step <= 1024) {
					count++;
					step *= 2;
				}
			}

			GetInstance().m_Exercise2_Timings.resize(count);

			for (int index = 0; index < GetInstance().m_Exercise2_Samples; ++index) {
				int step = 1;
				int c = 0;
				while (step <= 1024) {
					const auto start = std::chrono::high_resolution_clock::now();
					for (int i = 0; i < length; i += step) arr[i].ID *= 2;
					step *= 2;
					const auto end = std::chrono::high_resolution_clock::now();
					const auto total = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
					std::cout << step << " " << total << "\n";
					GetInstance().m_Exercise2_Timings[c] += (int)total;
					c++;
				}
			}
			for (int index = 0; index < (int)GetInstance().m_Exercise2_Timings.size(); ++index) {
				GetInstance().m_Exercise2_Timings[index] /= GetInstance().m_Exercise2_Samples;
			}
			delete[] arr;
		}
		if (!GetInstance().m_Exercise2_Timings.empty()) {
			ImPlot::BeginPlot("##timings_plot2");
			ImPlot::PlotLine("##timings2", GetInstance().m_Exercise2_Timings.data(), (int)GetInstance().m_Exercise2_Timings.size());
			ImPlot::EndPlot();
		}
	}
	ImGui::End();
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y) const
{
	SDL_FRect dst{};
	dst.x = x;
	dst.y = y;
	SDL_GetTextureSize(texture.GetSDLTexture(), &dst.w, &dst.h);
	SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height) const
{
	SDL_FRect dst{};
	dst.x = x;
	dst.y = y;
	dst.w = width;
	dst.h = height;
	SDL_RenderTexture(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

SDL_Renderer* dae::Renderer::GetSDLRenderer() const { return m_renderer; }
