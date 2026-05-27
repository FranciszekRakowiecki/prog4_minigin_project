#include "Game.h"
#if USE_STEAMWORKS
#include <steam_api.h>
#endif

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Component.h"

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextObject.h"
#include "Scene.h"

#include <filesystem>
namespace fs = std::filesystem;

int main(int, char*[]) {
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
    return 0;
}
