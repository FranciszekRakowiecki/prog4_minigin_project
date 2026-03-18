//
// Created by frane on 3/18/2026.
//

#pragma once
#ifndef MINIGIN_DATADISPLAYCOMPONENTS_H
#define MINIGIN_DATADISPLAYCOMPONENTS_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Xinput.h>

#include "Component.h"
#include "Events.h"
#include "Font.h"
#include "Input.h"
#include "SteamAchievements.h"
#include "Text.h"

#if USE_STEAMWORKS
#pragma warning (push)
#pragma warning (disable:4996)
#include <steam_api.h>
#pragma warning (pop)
#endif

class LivesScoreRenderer;

class ScoreTracker : public IObserver {
public:
    CSteamAchievements* achievements;
    LivesScoreRenderer* parent;
    void notify(Subject *subject, const Event *event, void *data) override;
};

class HealthTracker : public IObserver {
public:
    LivesScoreRenderer* parent;
    void notify(Subject *subject, const Event *event, void *data) override;
};

class DeathTracker : public IObserver {
public:
    void notify(Subject *subject, const Event *event, void *data) override;
};

class PlayerHealth final : public dae::Component {
    int m_health{3};
    int m_score{0};
public:
    Subject playerHealthEvents;

    void setHealth(int health) {
        m_health = health;
        if (m_health <= 0) {
            PlayerDeathEvent event{ this, m_health };
            playerHealthEvents.callEvent(&GameEvents::GetInstance().playerDeathEvent, &event);
        }
        PlayerHealthChanged health_changed{ this, m_health};
        playerHealthEvents.callEvent(&GameEvents::GetInstance().playerHealthChangedEvent, &health_changed);
    }
    int getHealth() const { return m_health; }
    int getScore() const { return m_score; }
    void addScore(int _score) {
        this->m_score += _score;
        PlayerScoreChanged score_changed{ this, m_score};
        playerHealthEvents.callEvent(&GameEvents::GetInstance().playerScoreChangedEvent, &score_changed);
    }
};

class LivesScoreRenderer final : public dae::Component {
    dae::GameObject* m_playerOne{nullptr};
    dae::GameObject* m_playerTwo{nullptr};
    std::shared_ptr<dae::Font> m_font{};

    std::unique_ptr<dae::Text> m_livesOne{nullptr};
    std::unique_ptr<dae::Text> m_livesTwo{nullptr};

    std::unique_ptr<dae::Text> m_scoreOne{nullptr};
    std::unique_ptr<dae::Text> m_scoreTwo{nullptr};

    dae::Reference<PlayerHealth> m_refHealthOne;
    dae::Reference<PlayerHealth> m_refHealthTwo;

    std::unique_ptr<ScoreTracker> m_scoreTracker;
    std::unique_ptr<HealthTracker> m_healthTracker;
    std::unique_ptr<DeathTracker> m_deathTracker;

    const dae::InputKey* m_PickupSmallKeyboard{dae::Input::KEY(SDL_SCANCODE_Z)};
    const dae::InputKey* m_PickupBigKeyboard{dae::Input::KEY(SDL_SCANCODE_X)};
    const dae::InputKey* m_TakeDamageKeyboard{dae::Input::KEY(SDL_SCANCODE_C)};

    const dae::InputGamepadButton* m_PickupSmallGamepad{dae::Input::GAMEPAD_BUTTON(XINPUT_GAMEPAD_A)};
    const dae::InputGamepadButton* m_PickupBigGamepad{dae::Input::GAMEPAD_BUTTON(XINPUT_GAMEPAD_B)};
    const dae::InputGamepadButton* m_TakeDamageGamepad{dae::Input::GAMEPAD_BUTTON(XINPUT_GAMEPAD_X)};

public:
    LivesScoreRenderer();
    void setPlayers(dae::GameObject* playerOne, dae::GameObject* playerTwo);
    void setFont(std::shared_ptr<dae::Font> font);
    void Render() override;
    void Update() override;

    void setAchievements(CSteamAchievements* achievements);

    void updateHealthFor(int health, PlayerHealth* player);
    void updateScoreFor(int health, PlayerHealth* player);

    void initText();

    int GetFlags() override;
};

#endif //MINIGIN_DATADISPLAYCOMPONENTS_H