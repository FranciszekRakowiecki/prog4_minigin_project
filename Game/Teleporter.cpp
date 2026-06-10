//
// Created by frane on 6/10/2026.
//

#include "Teleporter.h"
#include <glm/vec2.hpp>

#include "Minigin.h"

int Teleporter::GetFlags() {
    return COMPONENT_HAS_RENDER | COMPONENT_HAS_UPDATE;
}

void Teleporter::Update() {

}

void Teleporter::Render() {

}

void Teleporter::SetLevelData(const LevelData *data) {
    m_LevelData = data;
}

void Teleporter::SetPlayers(const std::vector<dae::GameObject *> &players) {
    m_Players = players;
}
