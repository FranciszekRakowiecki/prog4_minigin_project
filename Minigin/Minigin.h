#pragma once
#include <string>
#include <functional>
#include <filesystem>

#include "Input.h"
#include "SteamAchievements.h"

#if USE_STEAMWORKS
#pragma warning (push)
#pragma warning (disable:4996)
#include <steam_api.h>
#pragma warning (pop)
#endif

namespace dae
{
	class Minigin final
	{
		Input* m_Input;

		bool m_quit{};

		#if USE_STEAMWORKS
		std::unique_ptr<CSteamAchievements> m_Achievements;
		#endif

		float m_LastFrame{};
		const float m_TargetMS{ 1.0f / 60.0f };
	public:
		explicit Minigin(const std::filesystem::path& dataPath);
		~Minigin();
		void Run(const std::function<void(CSteamAchievements* achievements)>& load);
		void RunOneFrame();

		void Stop();

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;
	};
}
