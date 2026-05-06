//
// Created by frane on 5/6/2026.
//

#ifndef PROG4MINIGINPROJECT_GAME_H
#define PROG4MINIGINPROJECT_GAME_H
#include <memory>

#include "EngineHook.h"
#include "GameState.h"
#include "Singleton.h"


class Game final : public dae::Singleton<Game>, public dae::EngineHook {
private: // Avail game states
    std::unique_ptr<NullGameState> m_NullGameState;
    std::unique_ptr<MainMenuState> m_MainMenuState;
    std::unique_ptr<SinglePlayerState> m_SinglePlayerState;
    std::unique_ptr<CoopPlayerState> m_CoopPlayerState;
    std::unique_ptr<VersusPlayerState> m_VersusPlayerState;
public:
    Game();
    void ChangeState(GameState* state);

    void Start() override;

    void Update() override;

    void Render() override;

    void LoadMainMenu();
    void LoadSinglePlayer();
    void LoadCoopPlayer();
    void LoadVersusPlayer();

private:
    GameState* m_GameState;
};


#endif //PROG4MINIGINPROJECT_GAME_H