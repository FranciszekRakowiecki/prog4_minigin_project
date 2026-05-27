#include "Game.h"
#if USE_STEAMWORKS
#include <steam_api.h>
#endif

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "../Minigin/Component.h"
#include "../Minigin/DataDisplayComponents.h"

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "../Minigin/Minigin.h"
#include "../Minigin/SceneManager.h"
#include "../Minigin/ResourceManager.h"
#include "../Minigin/TextObject.h"
#include "../Minigin/Scene.h"

#include <filesystem>
namespace fs = std::filesystem;

int main(int, char*[]) {
#if USE_STEAMWORKS
	if (!SteamAPI_Init())
		std::cerr << (std::string("Fatal Error - Steam must be running to play this game (SteamAPI_Init() failed).")) << std::endl; // what about emscripten wont it crash because there is no steam launched?
#endif
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	engine.SetHook(&Game::GetInstance());
	engine.Run();

#if USE_STEAMWORKS
	SteamAPI_Shutdown();
#endif
    return 0;
}
