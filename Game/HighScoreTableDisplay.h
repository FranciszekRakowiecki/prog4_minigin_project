//
// Created by frane on 6/10/2026.
//

#ifndef PROG4MINIGINPROJECT_HIGHSCORETABLEDISPLAY_H
#define PROG4MINIGINPROJECT_HIGHSCORETABLEDISPLAY_H

#include "Component.h"

#include <string>

class TextRenderer;

class HighScoreTableDisplay : public dae::Component {
public:
    int GetFlags() override;
    void Start() override;
    void Update() override;

    void SetShowPlayerScores(bool showPlayerScores);
    void SetTitle(const std::string& title);
    void Refresh();

private:
    TextRenderer* m_TextRenderer{nullptr};
    bool m_ShowPlayerScores{true};
    std::string m_Title{"HIGH SCORES"};

    std::string BuildText() const;
};

#endif //PROG4MINIGINPROJECT_HIGHSCORETABLEDISPLAY_H
