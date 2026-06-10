//
// Created by frane on 6/10/2026.
//

#include "HighScoreTableDisplay.h"

#include <iomanip>
#include <sstream>

#include "Game.h"
#include "GameObject.h"
#include "ScoreKeeper.h"
#include "TextRenderer.h"

int HighScoreTableDisplay::GetFlags() {
    return COMPONENT_HAS_UPDATE;
}

void HighScoreTableDisplay::Start() {
    dae::Reference<TextRenderer> textRenderer = GetParent()->GetComponent<TextRenderer>();
    m_TextRenderer = textRenderer.get();
    Refresh();
}

void HighScoreTableDisplay::Update() {
    Refresh();
}

void HighScoreTableDisplay::SetShowPlayerScores(bool showPlayerScores) {
    m_ShowPlayerScores = showPlayerScores;
    Refresh();
}

void HighScoreTableDisplay::SetTitle(const std::string& title) {
    m_Title = title;
    Refresh();
}

void HighScoreTableDisplay::Refresh() {
    if (m_TextRenderer == nullptr) {
        dae::Reference<TextRenderer> textRenderer = GetParent()->GetComponent<TextRenderer>();
        m_TextRenderer = textRenderer.get();
        if (m_TextRenderer == nullptr) {
            return;
        }
    }

    const std::string text = BuildText();

    m_TextRenderer->SetText(text);
}

std::string HighScoreTableDisplay::BuildText() const {
    ScoreKeeper& scoreKeeper = Game::GetInstance().GetScoreKeeper();
    const auto& highScores = scoreKeeper.GetHighScores();

    std::ostringstream text{};
    text << m_Title << '\n';

    for (uint32_t index = 0; index < ScoreKeeper::HIGH_SCORE_COUNT; ++index) {
        const ScoreKeeper::HighScoreEntry& entry = highScores[index];
        text << std::setw(2) << index + 1 << ". ";

        if (entry.score == 0) {
            text << "--- 0";
        }
        else {
            text << std::left << std::setw(12) << entry.name << std::right << ' ' << entry.score;
        }

        if (index + 1 < ScoreKeeper::HIGH_SCORE_COUNT || m_ShowPlayerScores) {
            text << '\n';
        }
    }

    if (m_ShowPlayerScores) {
        text << '\n';
        for (uint32_t playerIndex = 0; playerIndex < ScoreKeeper::PLAYER_COUNT; ++playerIndex) {
            text << "P" << playerIndex + 1 << " " << scoreKeeper.GetPlayerScore(playerIndex);
            if (playerIndex + 1 < ScoreKeeper::PLAYER_COUNT) {
                text << '\n';
            }
        }
    }

    return text.str();
}
