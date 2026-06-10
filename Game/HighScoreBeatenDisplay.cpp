//
// Created by frane on 6/10/2026.
//

#include "HighScoreBeatenDisplay.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include "Game.h"
#include "GameTime.h"
#include "GameObject.h"
#include "PlayerInputHandler.h"
#include "PlayerInputManager.h"
#include "ScoreKeeper.h"
#include "TextRenderer.h"

int HighScoreBeatenDisplay::GetFlags() {
    return COMPONENT_HAS_UPDATE;
}

void HighScoreBeatenDisplay::Start() {
    m_TextRenderer = GetParent()->GetComponent<TextRenderer>();
    m_SubmitDelayTimer = m_SubmitDelay;
    Refresh();
}

void HighScoreBeatenDisplay::Update() {
    if (UpdateNameInput()) {
        Game::GetInstance().LoadMainMenu();
        return;
    }

    Refresh();
}

void HighScoreBeatenDisplay::SetPlayerIndex(uint32_t playerIndex) {
    m_PlayerIndex = playerIndex;
    Refresh();
}

void HighScoreBeatenDisplay::SetPlayerScore(uint32_t score) {
    m_CandidateScore = score;
    m_UsePlayerScore = false;
    Refresh();
}

void HighScoreBeatenDisplay::UsePlayerScore(bool usePlayerScore) {
    m_UsePlayerScore = usePlayerScore;
    Refresh();
}

void HighScoreBeatenDisplay::SetTextWhenBeaten(const std::string& text) {
    m_TextWhenBeaten = text;
    Refresh();
}

void HighScoreBeatenDisplay::SetTextWhenNotBeaten(const std::string& text) {
    m_TextWhenNotBeaten = text;
    Refresh();
}

void HighScoreBeatenDisplay::SetNameLength(uint32_t nameLength) {
    nameLength = std::clamp(nameLength, 1u, 12u);
    m_EnteredName.resize(nameLength, 'A');
    if (m_SelectedNameIndex >= nameLength) {
        m_SelectedNameIndex = nameLength - 1;
    }
    Refresh();
}

void HighScoreBeatenDisplay::SetMoveRepeatDelay(float delay) {
    m_MoveRepeatDelay = std::max(0.0f, delay);
}

void HighScoreBeatenDisplay::SetSubmitDelay(float delay) {
    m_SubmitDelay = std::max(0.0f, delay);
    m_SubmitDelayTimer = m_SubmitDelay;
}

void HighScoreBeatenDisplay::Refresh() {
    if (!m_TextRenderer) {
        m_TextRenderer = GetParent()->GetComponent<TextRenderer>();
        if (!m_TextRenderer) {
            return;
        }
    }

    ScoreKeeper& scoreKeeper = Game::GetInstance().GetScoreKeeper();
    const bool highScoreBeaten = scoreKeeper.IsHighScore(GetCurrentScore());

    const std::string text = highScoreBeaten ? BuildBeatenText() : m_TextWhenNotBeaten;

    m_TextRenderer->SetText(text);
}

bool HighScoreBeatenDisplay::UpdateNameInput() {
    if (m_Submitted || !Game::GetInstance().GetScoreKeeper().IsHighScore(GetCurrentScore())) {
        return false;
    }

    const PlayerInputHandler* input = GetPlayerInput();
    if (input == nullptr) {
        return false;
    }

    if (m_SubmitDelayTimer > 0.0f) {
        m_SubmitDelayTimer -= dae::GameTime::GetInstance().GetDeltaTime();
        return false;
    }

    if (input->IsShooting()) {
        Submit();
        return true;
    }

    m_MoveRepeatTimer -= dae::GameTime::GetInstance().GetDeltaTime();

    const glm::vec2 move = input->GetMove();
    const bool hasMoveInput = std::abs(move.x) > 0.5f || std::abs(move.y) > 0.5f;
    if (!hasMoveInput) {
        m_MoveRepeatTimer = 0.0f;
        return false;
    }

    if (m_MoveRepeatTimer > 0.0f) {
        return false;
    }

    if (std::abs(move.x) > std::abs(move.y)) {
        MoveSelectedSlot(move.x > 0.0f ? 1 : -1);
    }
    else {
        CycleSelectedLetter(move.y > 0.0f ? 1 : -1);
    }

    m_MoveRepeatTimer = m_MoveRepeatDelay;
    return false;
}

void HighScoreBeatenDisplay::CycleSelectedLetter(int offset) {
    static const std::string characters{"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};
    if (m_EnteredName.empty()) {
        return;
    }

    const char currentLetter = m_EnteredName[m_SelectedNameIndex];
    const size_t currentIndex = characters.find(currentLetter);
    const int safeIndex = currentIndex == std::string::npos ? 0 : int(currentIndex);
    const int nextIndex = (safeIndex + offset + int(characters.size())) % int(characters.size());
    m_EnteredName[m_SelectedNameIndex] = characters[size_t(nextIndex)];
}

void HighScoreBeatenDisplay::MoveSelectedSlot(int offset) {
    if (m_EnteredName.empty()) {
        return;
    }

    const int nextIndex = (int(m_SelectedNameIndex) + offset + int(m_EnteredName.size())) % int(m_EnteredName.size());
    m_SelectedNameIndex = uint32_t(nextIndex);
}

void HighScoreBeatenDisplay::Submit() {
    m_Submitted = true;

    if (m_UsePlayerScore) {
        Game::GetInstance().GetScoreKeeper().SubmitPlayerHighScore(m_PlayerIndex, m_EnteredName);
    }
    else {
        Game::GetInstance().GetScoreKeeper().SubmitHighScore(m_EnteredName, m_CandidateScore);
    }

}

uint32_t HighScoreBeatenDisplay::GetCurrentScore() const {
    if (m_UsePlayerScore) {
        return Game::GetInstance().GetScoreKeeper().GetPlayerScore(m_PlayerIndex);
    }

    return m_CandidateScore;
}

std::string HighScoreBeatenDisplay::BuildBeatenText() const {
    std::string text = m_TextWhenBeaten;
    text += '\n';

    for (uint32_t index = 0; index < m_EnteredName.size(); ++index) {
        if (index == m_SelectedNameIndex) {
            text += '[';
            text += m_EnteredName[index];
            text += ']';
        }
        else {
            text += ' ';
            text += m_EnteredName[index];
            text += ' ';
        }
    }

    text += "\nSHOOT TO SUBMIT";
    return text;
}

const PlayerInputHandler* HighScoreBeatenDisplay::GetPlayerInput() const {
    const std::vector<PlayerInputHandler>& players = PlayerInputManager::GetInstance().GetPlayers();
    for (const PlayerInputHandler& player : players) {
        if (player.GetPlayerIndex() == m_PlayerIndex) {
            return &player;
        }
    }

    if (m_PlayerIndex < players.size()) {
        return &players[m_PlayerIndex];
    }

    return nullptr;
}
