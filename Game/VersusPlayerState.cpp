//
// Created by frane on 5/27/2026.
//

#include "VersusPlayerState.h"

#include "PlayerInputManager.h"
#include "ProjectileSystem.h"

void VersusPlayerState::OnLevelLoad(LevelData *level_data, uint32_t uint32) {
    SetCompleteLevelWhenEnemiesDestroyed(false);
    SetEndGameWhenOnePlayerRemains(true);

    const std::vector<PlayerInputHandler>& players = PlayerInputManager::GetInstance().GetPlayers();
    if (players.size() >= 2) {
        SpawnPlayer(&players[0]);
        SpawnPlayer(&players[1]);
    }
    GetProjectileSystem()->SetPlayerProjectilesHitPlayers(true);
}
