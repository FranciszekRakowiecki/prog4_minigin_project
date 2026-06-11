//
// Created by frane on 5/27/2026.
//

#include "CoopPlayerState.h"

#include <vector>

#include "PlayerInputManager.h"

class PlayerInputHandler;

void CoopPlayerState::OnLevelLoad(LevelData*, uint32_t) {
    const std::vector<PlayerInputHandler>& players = PlayerInputManager::GetInstance().GetPlayers();
    if (!players.empty()) {
        SpawnPlayer(&players[0]);
        SpawnPlayer(&players[1]);
    }

    SpawnEnemies();
}
