//
// Created by frane on 5/27/2026.
//

#include "SinglePlayerState.h"

#include <vector>

#include "PlayerInputManager.h"

void SinglePlayerState::OnLevelLoad(LevelData*, uint32_t) {
    const std::vector<PlayerInputHandler>& players = PlayerInputManager::GetInstance().GetPlayers();
    if (!players.empty()) {
        SpawnPlayer(&players[0]);
    }

    SpawnEnemies();
}
