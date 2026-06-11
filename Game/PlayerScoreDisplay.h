//
// Created by frane on 6/11/2026.
//

#ifndef PROG4MINIGINPROJECT_PLAYERSCOREDISPLAY_H
#define PROG4MINIGINPROJECT_PLAYERSCOREDISPLAY_H

#include "Component.h"

#include <cstdint>
#include <limits>
#include <string>

class TextRenderer;

class PlayerScoreDisplay : public dae::Component {
public:
    int GetFlags() override;
    void Start() override;
    void Update() override;

    void SetPlayerIndex(uint32_t playerIndex);
    void SetPrefix(const std::string& prefix);

private:
    dae::Reference<TextRenderer> m_TextRenderer{};
    uint32_t m_PlayerIndex{};
    std::string m_Prefix{"Score: "};
    uint32_t m_LastScore{std::numeric_limits<uint32_t>::max()};

    void Refresh();
};

#endif //PROG4MINIGINPROJECT_PLAYERSCOREDISPLAY_H
