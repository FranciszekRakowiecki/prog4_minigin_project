//
// Created by frane on 6/10/2026.
//

#include "ScoreKeeper.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <SDL3/SDL_filesystem.h>

uint32_t ScoreKeeper::GetPlayerScore(uint32_t index) const {
    if (index >= PLAYER_COUNT) {
        return 0;
    }

    return m_PlayerScores[index];
}

void ScoreKeeper::AddPlayerScore(uint32_t index, uint32_t playerScore) {
    if (index >= PLAYER_COUNT) {
        return;
    }

    m_PlayerScores[index] += playerScore;
}

void ScoreKeeper::SetPlayerScore(uint32_t index, uint32_t playerScore) {
    if (index >= PLAYER_COUNT) {
        return;
    }

    m_PlayerScores[index] = playerScore;
}

void ScoreKeeper::ResetPlayerScores() {
    m_PlayerScores.fill(0);
}

void ScoreKeeper::LoadHighScores() {
    m_HighScores = {};

    std::ifstream file{std::filesystem::current_path() / "highscores.txt"};
    if (!file.is_open()) {
        return;
    }

    std::string line{};
    uint32_t index{};
    while (index < HIGH_SCORE_COUNT && std::getline(file, line)) {
        std::istringstream lineStream{line};
        uint32_t score{};
        std::string name{};

        if (!(lineStream >> score)) {
            continue;
        }

        std::getline(lineStream >> std::ws, name);
        m_HighScores[index] = {VerifyName(name), score};
        ++index;
    }

    std::sort(m_HighScores.begin(), m_HighScores.end(), [](const HighScoreEntry& first, const HighScoreEntry& second) {
        return first.score > second.score;
    });
}

void ScoreKeeper::SaveHighScores() {
    std::ofstream file{std::filesystem::current_path() / "highscores.txt", std::ios::trunc};
    if (!file.is_open()) {
        return;
    }

    for (const HighScoreEntry& entry : m_HighScores) {
        if (entry.score == 0) {
            continue;
        }
        file << entry.score << ' ' << VerifyName(entry.name) << '\n';
    }
}

bool ScoreKeeper::SubmitHighScore(const std::string& name, uint32_t score) {
    if (score == 0) {
        return false;
    }

    uint32_t insertIndex = HIGH_SCORE_COUNT;
    for (uint32_t index = 0; index < HIGH_SCORE_COUNT; ++index) {
        if (score > m_HighScores[index].score) {
            insertIndex = index;
            break;
        }
    }

    if (insertIndex == HIGH_SCORE_COUNT) {
        return false;
    }

    for (uint32_t index = HIGH_SCORE_COUNT - 1; index > insertIndex; --index) {
        m_HighScores[index] = m_HighScores[index - 1];
    }

    m_HighScores[insertIndex] = {VerifyName(name), score};
    SaveHighScores();
    return true;
}

bool ScoreKeeper::SubmitPlayerHighScore(uint32_t playerIndex, const std::string& name) {
    return SubmitHighScore(name, GetPlayerScore(playerIndex));
}

const std::array<ScoreKeeper::HighScoreEntry, ScoreKeeper::HIGH_SCORE_COUNT>& ScoreKeeper::GetHighScores() const {
    return m_HighScores;
}

std::string ScoreKeeper::VerifyName(const std::string& name) {
    if (name.size() > 6)
        return "PLAYER";

    if (name.empty()) {
        return "PLAYER";
    }

    return name;
}
