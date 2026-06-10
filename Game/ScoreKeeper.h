//
// Created by frane on 6/10/2026.
//

#ifndef PROG4MINIGINPROJECT_SCOREKEEPER_H
#define PROG4MINIGINPROJECT_SCOREKEEPER_H
#include <cstdint>
#include <array>
#include <string>


class ScoreKeeper {
public:
    static constexpr uint32_t PLAYER_COUNT{2};
    static constexpr uint32_t HIGH_SCORE_COUNT{10};

    struct HighScoreEntry {
        std::string name{"---"};
        uint32_t score{};
    };

    uint32_t GetPlayerScore(uint32_t index) const;
    void AddPlayerScore(uint32_t index, uint32_t playerScore);
    void SetPlayerScore(uint32_t index, uint32_t playerScore);
    void ResetPlayerScores();

    void LoadHighScores();
    void SaveHighScores();
    bool SubmitHighScore(const std::string& name, uint32_t score);
    bool SubmitPlayerHighScore(uint32_t playerIndex, const std::string& name);
    const std::array<HighScoreEntry, HIGH_SCORE_COUNT>& GetHighScores() const;

private:
    std::array<uint32_t, PLAYER_COUNT> m_PlayerScores{};
    std::array<HighScoreEntry, HIGH_SCORE_COUNT> m_HighScores{};

    static std::string VerifyName(const std::string& name);
};


#endif //PROG4MINIGINPROJECT_SCOREKEEPER_H
