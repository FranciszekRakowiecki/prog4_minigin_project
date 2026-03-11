#pragma once
#include <string>
#include <functional>
#include <filesystem>

#include "Input.h"

namespace dae
{
	class Minigin final
	{
		Input* m_Input;

		bool m_quit{};

		float m_LastFrame{};
		const float m_TargetMS{ 1.0f / 60.0f };
	public:
		explicit Minigin(const std::filesystem::path& dataPath);
		~Minigin();
		void Run(const std::function<void()>& load);
		void RunOneFrame();

		void Stop();

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;
	};
}
