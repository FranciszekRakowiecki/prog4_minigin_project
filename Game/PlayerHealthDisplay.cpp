//
// Created by frane on 6/10/2026.
//

#include "PlayerHealthDisplay.h"

#include <string>

#include "GameObject.h"
#include "TextRenderer.h"

int PlayerHealthDisplay::GetFlags() {
    return COMPONENT_HAS_UPDATE;
}

void PlayerHealthDisplay::Start() {
    m_TextRenderer = GetParent()->GetComponent<TextRenderer>();
    Refresh();
}

void PlayerHealthDisplay::Update() {
    Refresh();
}

void PlayerHealthDisplay::SetPlayerHealth(dae::Reference<PlayerHealth> playerHealth) {
    m_PlayerHealth = playerHealth;
    m_LastLives = -1;
    Refresh();
}

void PlayerHealthDisplay::SetPrefix(const std::string& prefix) {
    m_Prefix = prefix;
    m_LastLives = -1;
    Refresh();
}

void PlayerHealthDisplay::Refresh() {
    if (!m_TextRenderer) {
        m_TextRenderer = GetParent()->GetComponent<TextRenderer>();
        if (!m_TextRenderer) {
            return;
        }
    }

    if (!m_PlayerHealth) {
        m_TextRenderer->SetText(m_Prefix + "-");
        return;
    }

    if (m_PlayerHealth->lives == m_LastLives) {
        return;
    }

    m_LastLives = m_PlayerHealth->lives;
    m_TextRenderer->SetText(m_Prefix + std::to_string(m_LastLives));
}
