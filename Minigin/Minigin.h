#pragma once
#include <string>
#include <functional>
#include <filesystem>

#include "EngineHook.h"
#include "Input.h"
#include "SteamAchievements.h"

#if USE_STEAMWORKS
namespace dae {
	class EngineHook;
}
#include <steam_api.h>
#endif

#if __EMSCRIPTEN__
#define MINIGIN_DATA_PATH std::string("")
#else
#define MINIGIN_DATA_PATH std::string("./Data/")
#endif

namespace dae
{
	class Minigin final
	{
		Input* m_Input;

		bool m_quit{};

		std::unique_ptr<CSteamAchievements> m_Achievements;

		float m_LastFrame{};
		const float m_TargetMS{ 1.0f / 60.0f };
		EngineHook* m_EngineHook;

	public:
		explicit Minigin(const std::filesystem::path& dataPath);
		~Minigin();
		void Run(const std::function<void(CSteamAchievements* achievements)>& load);
		void RunOneFrame();

		void SetHook(EngineHook* hook);

		void Stop();

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;
	};
}
