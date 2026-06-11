//
// Created by frane on 6/11/2026.
//

#include "PlayerScoreDisplay.h"

#include <limits>

#include "Game.h"
#include "GameObject.h"
#include "TextRenderer.h"

int PlayerScoreDisplay::GetFlags() {
    return COMPONENT_HAS_UPDATE;
}

void PlayerScoreDisplay::Start() {
    m_TextRenderer = GetParent()->GetComponent<TextRenderer>();
    Refresh();
}

void PlayerScoreDisplay::Update() {
    Refresh();
}

void PlayerScoreDisplay::SetPlayerIndex(uint32_t playerIndex) {
    m_PlayerIndex = playerIndex;
    m_LastScore = std::numeric_limits<uint32_t>::max();
    Refresh();
}

void PlayerScoreDisplay::SetPrefix(const std::string& prefix) {
    m_Prefix = prefix;
    m_LastScore = std::numeric_limits<uint32_t>::max();
    Refresh();
}

void PlayerScoreDisplay::Refresh() {
    if (!m_TextRenderer) {
        m_TextRenderer = GetParent()->GetComponent<TextRenderer>();
        if (!m_TextRenderer) {
            return;
        }
    }

    const uint32_t score = Game::GetInstance().GetScoreKeeper().GetPlayerScore(m_PlayerIndex);
    if (score == m_LastScore) {
        return;
    }

    m_LastScore = score;
    m_TextRenderer->SetText(m_Prefix + std::to_string(score));
}
