//
// Created by frane on 6/10/2026.
//

#ifndef PROG4MINIGINPROJECT_HIGHSCOREBEATENDISPLAY_H
#define PROG4MINIGINPROJECT_HIGHSCOREBEATENDISPLAY_H

#include "Component.h"

#include <cstdint>
#include <string>

class TextRenderer;

class HighScoreBeatenDisplay : public dae::Component {
public:
    int GetFlags() override;
    void Start() override;
    void Update() override;

    void SetPlayerIndex(uint32_t playerIndex);
    void SetPlayerScore(uint32_t score);
    void UsePlayerScore(bool usePlayerScore);
    void SetTextWhenBeaten(const std::string& text);
    void SetTextWhenNotBeaten(const std::string& text);
    void SetNameLength(uint32_t nameLength);
    void SetMoveRepeatDelay(float delay);
    void SetSubmitDelay(float delay);

private:
    dae::Reference<TextRenderer> m_TextRenderer{};
    uint32_t m_PlayerIndex{};
    uint32_t m_CandidateScore{};
    bool m_UsePlayerScore{true};
    std::string m_TextWhenBeaten{"NEW HIGH SCORE"};
    std::string m_TextWhenNotBeaten{};
    std::string m_EnteredName{"AAA"};
    uint32_t m_SelectedNameIndex{};
    float m_MoveRepeatDelay{0.18f};
    float m_MoveRepeatTimer{};
    float m_SubmitDelay{0.5f};
    float m_SubmitDelayTimer{0.5f};
    bool m_Submitted{};

    void Refresh();
    bool UpdateNameInput();
    void CycleSelectedLetter(int offset);
    void MoveSelectedSlot(int offset);
    void Submit();
    uint32_t GetCurrentScore() const;
    std::string BuildBeatenText() const;
    const class PlayerInputHandler* GetPlayerInput() const;
};

#endif //PROG4MINIGINPROJECT_HIGHSCOREBEATENDISPLAY_H
