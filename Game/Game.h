//
// Created by frane on 5/6/2026.
//

#ifndef PROG4MINIGINPROJECT_GAME_H
#define PROG4MINIGINPROJECT_GAME_H
#include <memory>

#include "EngineHook.h"
#include "GameState.h"
#include "Singleton.h"

#include "CoopPlayerState.h"
#include "Font.h"
#include "MainMenuState.h"
#include "SinglePlayerState.h"
#include "VersusPlayerState.h"

class Game final : public dae::Singleton<Game>, public dae::EngineHook {
public:
    const int FONT_SIZE{ 30 };

    Game();
    void ChangeState(GameState* state);

    void Start() override;

    void Update() override;

    void Render() override;

    void LoadMainMenu();
    void LoadSinglePlayer();
    void LoadCoopPlayer();
    void LoadVersusPlayer();

    std::shared_ptr<dae::Font> GetGameFont() const;

private:
    std::unique_ptr<NullGameState> m_NullGameState;
    std::unique_ptr<MainMenuState> m_MainMenuState;
    std::unique_ptr<SinglePlayerState> m_SinglePlayerState;
    std::unique_ptr<CoopPlayerState> m_CoopPlayerState;
    std::unique_ptr<VersusPlayerState> m_VersusPlayerState;

    std::shared_ptr<dae::Font> m_GameFont;
    GameState* m_GameState;
};


#endif //PROG4MINIGINPROJECT_GAME_H