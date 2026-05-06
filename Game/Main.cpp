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

static void load(CSteamAchievements* achievements)
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	auto go = std::make_unique<dae::GameObject>();
	go->SetTexture("background.png");
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTexture("logo.png");
	go->SetPosition(358, 180);
	scene.Add(std::move(go));

	dae::GameObject* playerOne;
	dae::GameObject* playerTwo;

	go = std::make_unique<dae::GameObject>();
	playerOne = go.get();
	go->SetTexture("packerman.png");
	go->AddComponent<PlayerHealth>();
	go->SetPosition(400, 180);
	go->AddComponent<dae::ExampleMovementKeyboard>();
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	playerTwo = go.get();
	go->SetTexture("packerman.png");
	go->AddComponent<PlayerHealth>();
	go->SetPosition(378, 180);
	go->AddComponent<dae::ExampleMovementDPAD>();
	scene.Add(std::move(go));

	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto to = std::make_unique<dae::TextObject>("Programming 4 Assignment", font);
	to->SetColor({ 255, 255, 0, 255 });
	to->SetPosition(292, 20);
	scene.Add(std::move(to));

	font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
	to = std::make_unique<dae::TextObject>("Use the D-Pad to move Pacman, X to inflict damage, A and B to pickup pellets", font);
	to->SetColor({ 200, 200, 200, 255 });
	to->SetPosition(0, 60);
	scene.Add(std::move(to));
	// font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
	to = std::make_unique<dae::TextObject>("Use the WSAD to move Pacman, C to inflict damage, Z and X to pickup pellets", font);
	to->SetColor({ 200, 200, 200, 255 });
	to->SetPosition(0, 80);
	scene.Add(std::move(to));

	go = std::make_unique<dae::GameObject>();
	dae::Reference<LivesScoreRenderer> renderer = go->AddComponent<LivesScoreRenderer>();
	renderer->setPlayers(playerOne,playerTwo);
	renderer->setFont(font);
	renderer->initText();
	renderer->setAchievements(achievements);
	go->SetPosition(0, 100);
	scene.Add(std::move(go));

	auto fps = std::make_unique<dae::TextObject>("FPS: ", font);
	fps->SetColor({ 255, 255, 255, 255 });
	fps->SetPosition(0, 0);
	fps->AddComponent<dae::ExampleUpdateComponent>();

	scene.Add(std::move(fps));
}

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
	engine.Run(load);

#if USE_STEAMWORKS
	SteamAPI_Shutdown();
#endif
    return 0;
}
