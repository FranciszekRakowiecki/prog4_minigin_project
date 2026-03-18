//
// Created by frane on 3/18/2026.
//

#include "DataDisplayComponents.h"

#include <iostream>

#include "GameObject.h"
#include "Renderer.h"
#include "Text.h"

void ScoreTracker::notify(Subject *, const Event *event, void *data) {
    if (event == &GameEvents::GetInstance().playerScoreChangedEvent) {
        PlayerScoreChanged* eventData = static_cast<PlayerScoreChanged *>(data);

        if (eventData->value >= 500) {
            achievements->SetAchievement("ACH_WIN_100_GAMES");
        }

        parent->updateScoreFor(eventData->value, eventData->player);
    }
}

void HealthTracker::notify(Subject *, const Event *event, void *data) {
    if (event == &GameEvents::GetInstance().playerHealthChangedEvent) {
        PlayerHealthChanged* eventData = static_cast<PlayerHealthChanged *>(data);

        parent->updateHealthFor(eventData->value, eventData->player);
    }
}

void DeathTracker::notify(Subject *, const Event *event, void *) {
    if (event == &GameEvents::GetInstance().playerDeathEvent) {
        // PlayerDeathEvent* eventData = static_cast<PlayerDeathEvent*>(data);

        std::cout << "Player died" << std::endl;
    }
}

LivesScoreRenderer::LivesScoreRenderer() : m_refHealthOne(), m_refHealthTwo() {
    m_scoreTracker = std::make_unique<ScoreTracker>();
    m_scoreTracker->parent = this;
    m_healthTracker = std::make_unique<HealthTracker>();
    m_healthTracker->parent = this;
    m_deathTracker = std::make_unique<DeathTracker>();

    m_livesOne = std::make_unique<dae::Text>();
    m_livesTwo = std::make_unique<dae::Text>();
    m_scoreOne = std::make_unique<dae::Text>();
    m_scoreTwo = std::make_unique<dae::Text>();
}

void LivesScoreRenderer::setPlayers(dae::GameObject *playerOne, dae::GameObject *playerTwo) {
    m_playerOne = playerOne;
    m_playerTwo = playerTwo;

    m_refHealthOne = m_playerOne->GetComponent<PlayerHealth>();
    m_refHealthTwo = m_playerTwo->GetComponent<PlayerHealth>();

    m_refHealthOne->playerHealthEvents.addObserver(m_scoreTracker.get());
    m_refHealthOne->playerHealthEvents.addObserver(m_healthTracker.get());
    m_refHealthOne->playerHealthEvents.addObserver(m_deathTracker.get());

    m_refHealthTwo->playerHealthEvents.addObserver(m_scoreTracker.get());
    m_refHealthTwo->playerHealthEvents.addObserver(m_healthTracker.get());
    m_refHealthTwo->playerHealthEvents.addObserver(m_deathTracker.get());
}

void LivesScoreRenderer::setFont(std::shared_ptr<dae::Font> font) {
    m_font = font;

    m_livesOne->SetFont(font);
    m_livesTwo->SetFont(font);
    m_scoreOne->SetFont(font);
    m_scoreTwo->SetFont(font);
}

void LivesScoreRenderer::Render() {
    dae::Transform& transform = GetParent()->transform;
    glm::vec3 position = transform.GetWorldPosition();

    dae::Renderer::GetInstance().RenderText(*m_livesOne, position.x, position.y);
    dae::Renderer::GetInstance().RenderText(*m_scoreOne, position.x, position.y + 36);
    dae::Renderer::GetInstance().RenderText(*m_livesTwo, position.x, position.y + 36 * 2);
    dae::Renderer::GetInstance().RenderText(*m_scoreTwo, position.x, position.y + 36 * 3);
}

void LivesScoreRenderer::Update() {
    if (m_PickupSmallKeyboard->pressedThisFrame()) {
        m_refHealthOne->addScore(10);
    }
    if (m_PickupBigKeyboard->pressedThisFrame()) {
        m_refHealthOne->addScore(100);
    }
    if (m_PickupSmallGamepad->pressedThisFrame()) {
        m_refHealthTwo->addScore(10);
    }
    if (m_PickupBigGamepad->pressedThisFrame()) {
        m_refHealthTwo->addScore(100);
    }
    if (m_TakeDamageKeyboard->pressedThisFrame()) {
        m_refHealthOne->setHealth(m_refHealthOne->getHealth() - 1);
    }
    if (m_TakeDamageGamepad->pressedThisFrame()) {
        m_refHealthTwo->setHealth(m_refHealthTwo->getHealth() - 1);
    }

    m_livesOne->updateTexture();
    m_livesTwo->updateTexture();
    m_scoreOne->updateTexture();
    m_scoreTwo->updateTexture();
}

void LivesScoreRenderer::setAchievements(CSteamAchievements *achievements) {
    m_scoreTracker->achievements = achievements;
}

void LivesScoreRenderer::updateHealthFor(int health, PlayerHealth* player) {
    if (player == m_refHealthOne.get()) {
        m_livesOne->SetText("# p1 lives: " + std::to_string(health));
    }
    else if (player == m_refHealthTwo.get()) {
        m_livesTwo->SetText("# p2 lives: " + std::to_string(health));
    }
}

void LivesScoreRenderer::updateScoreFor(int score, PlayerHealth* player) {
    if (player == m_refHealthOne.get()) {
        m_scoreOne->SetText("p1 score: " + std::to_string(score));
    }
    else if (player == m_refHealthTwo.get()) {
        m_scoreTwo->SetText("p2 score: " + std::to_string(score));
    }
}

void LivesScoreRenderer::initText() {
    updateHealthFor(m_refHealthOne.get()->getHealth(), m_refHealthOne.get());
    updateHealthFor(m_refHealthTwo.get()->getHealth(), m_refHealthTwo.get());
    updateScoreFor(m_refHealthOne.get()->getScore(), m_refHealthOne.get());
    updateScoreFor(m_refHealthTwo.get()->getScore(), m_refHealthTwo.get());
}

int LivesScoreRenderer::GetFlags() {
    return COMPONENT_HAS_RENDER | COMPONENT_HAS_UPDATE;
}
