//
// Created by frane on 5/27/2026.
//

#include "VersusPlayerState.h"

#include "PlayerInputManager.h"
#include "ProjectileSystem.h"

void VersusPlayerState::OnLevelLoad(LevelData *level_data, uint32_t uint32) {
    const std::vector<PlayerInputHandler>& players = PlayerInputManager::GetInstance().GetPlayers();
    if (!players.empty()) {
        SpawnPlayer(&players[0]);
        SpawnPlayer(&players[1]);
    }
    GetProjectileSystem()->SetPlayerProjectilesHitPlayers(true);
}
