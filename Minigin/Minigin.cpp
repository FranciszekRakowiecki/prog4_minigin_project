#include <stdexcept>
#include <sstream>
#include <iostream>

#if WIN32
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#endif

#include <SDL3/SDL.h>
//#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Minigin.h"

#include "GameTime.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "ServiceLocator.h"
#include "SoundLibrary.h"
#include "SoundSystemSDL.h"

SDL_Window* g_window{};

void LogSDLVersion(const std::string& message, int major, int minor, int patch)
{
#if WIN32
	std::stringstream ss;
	ss << message << major << "." << minor << "." << patch << "\n";
	OutputDebugString(ss.str().c_str());
#else
	std::cout << message << major << "." << minor << "." << patch << "\n";
#endif
}

#ifdef __EMSCRIPTEN__
#include "emscripten.h"

void LoopCallback(void* arg)
{
	static_cast<dae::Minigin*>(arg)->RunOneFrame();
}
#endif

// Why bother with this? Because sometimes students have a different SDL version installed on their pc.
// That is not a problem unless for some reason the dll's from this project are not copied next to the exe.
// These entries in the debug output help to identify that issue.
void PrintSDLVersion()
{
	LogSDLVersion("Compiled with SDL", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
	int version = SDL_GetVersion();
	LogSDLVersion("Linked with SDL ", SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version), SDL_VERSIONNUM_MICRO(version));
	// LogSDLVersion("Compiled with SDL_image ",SDL_IMAGE_MAJOR_VERSION, SDL_IMAGE_MINOR_VERSION, SDL_IMAGE_MICRO_VERSION);
	// version = IMG_Version();
	// LogSDLVersion("Linked with SDL_image ", SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version), SDL_VERSIONNUM_MICRO(version));
	LogSDLVersion("Compiled with SDL_ttf ",	SDL_TTF_MAJOR_VERSION, SDL_TTF_MINOR_VERSION,SDL_TTF_MICRO_VERSION);
	version = TTF_Version();
	LogSDLVersion("Linked with SDL_ttf ", SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version),	SDL_VERSIONNUM_MICRO(version));
}

dae::Minigin::Minigin(const std::filesystem::path& dataPath)
{
#if USE_STEAMWORKS
	static Achievement_t g_Achievements[] =
	{
		_ACH_ID( ACH_WIN_ONE_GAME, "Winner" ),
		_ACH_ID( ACH_WIN_100_GAMES, "Champion" ),
		_ACH_ID( ACH_TRAVEL_FAR_ACCUM, "Interstellar" ),
		_ACH_ID( ACH_TRAVEL_FAR_SINGLE, "Orbiter" ),
	};
	m_Achievements = std::make_unique<CSteamAchievements>(g_Achievements, 4);
#else
	static Achievement_t g_Achievements[] = { { 0, "a", "a", "a", false, 0 }};
	m_Achievements = std::make_unique<CSteamAchievements>(g_Achievements, 0);
#endif

	ServiceLocator::GetInstance().setSoundSystem(std::move(std::make_unique<SoundSystemSDL>()));

	PrintSDLVersion();

	if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
	{
		SDL_Log("Renderer error: %s", SDL_GetError());
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	g_window = SDL_CreateWindow(
		"Programming 4 assignment",
		1024,
		576,
		SDL_WINDOW_OPENGL
	);
	if (g_window == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	m_Input = new Input(this, g_window);

#if __EMSCRIPTEN__
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
#endif

	Renderer::GetInstance().Init(g_window);
	ResourceManager::GetInstance().Init(dataPath);

	ServiceLocator::GetInstance().Init();

	SoundLibrary::GetInstance().Load();
}

dae::Minigin::~Minigin()
{
	ServiceLocator::GetInstance().Shutdown();
	delete m_Input;
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(g_window);
	g_window = nullptr;
	SDL_Quit();
}

void dae::Minigin::Run(const std::function<void(CSteamAchievements* achievements)>& load)
{
	load(m_Achievements.get());
#ifndef __EMSCRIPTEN__
	m_LastFrame = GameTime::GetInstance().GetTime();

	while (!m_quit) {
		RunOneFrame();
	}
#else
	emscripten_set_main_loop_arg(&LoopCallback, this, 0, true);
#endif
}

void dae::Minigin::RunOneFrame()
{
#if USE_STEAMWORKS
	SteamAPI_RunCallbacks();
#endif
	float delta = GameTime::GetInstance().GetTime() - m_LastFrame;

	if (delta < m_TargetMS)
		return;

	m_Input->pollEvents();

	m_LastFrame = GameTime::GetInstance().GetTime();
	GameTime::GetInstance().SetDeltaTime(delta);
	// m_quit = !InputManager::GetInstance().ProcessInput();
	SceneManager::GetInstance().Update();
	ServiceLocator::GetInstance().update();
	Renderer::GetInstance().Render();
}

void dae::Minigin::Stop() {
	m_quit = true;
}
