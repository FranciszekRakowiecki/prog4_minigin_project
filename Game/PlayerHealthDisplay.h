//
// Created by frane on 6/10/2026.
//

#ifndef PROG4MINIGINPROJECT_PLAYERHEALTHDISPLAY_H
#define PROG4MINIGINPROJECT_PLAYERHEALTHDISPLAY_H

#include "Component.h"
#include "PlayerHealth.h"

#include <string>

class TextRenderer;

class PlayerHealthDisplay : public dae::Component {
public:
    int GetFlags() override;
    void Start() override;
    void Update() override;

    void SetPlayerHealth(dae::Reference<PlayerHealth> playerHealth);
    void SetPrefix(const std::string& prefix);

private:
    dae::Reference<PlayerHealth> m_PlayerHealth{};
    dae::Reference<TextRenderer> m_TextRenderer{};
    std::string m_Prefix{"Lives: "};
    int m_LastLives{-1};

    void Refresh();
};

#endif //PROG4MINIGINPROJECT_PLAYERHEALTHDISPLAY_H
