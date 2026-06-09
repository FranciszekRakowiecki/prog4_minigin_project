//
// Created by frane on 5/27/2026.
//

#ifndef PROG4MINIGINPROJECT_MAINMENUSTATE_H
#define PROG4MINIGINPROJECT_MAINMENUSTATE_H
#include "GameState.h"
#include <cstdint>
#include <vector>

#include "Component.h"
#include "PlayerInputHandler.h"
#include "TextRenderer.h"


class Button;

namespace dae {
    class Scene;
}

class MainMenuState : public GameState {
    dae::Scene* m_Scene{nullptr};
    int32_t m_ButtonSelection{0};
    float m_LastInputTime{0.0f};
    const float m_InputDelay{0.1f};

    std::vector<dae::Reference<Button>> m_Buttons;
    dae::Reference<TextRenderer> m_ErrorText;
    dae::Reference<TextRenderer> m_PlayerText0;
    dae::Reference<TextRenderer> m_PlayerText1;

    void CreateButton(const std::string &text, std::function<void()> callback);

    void CreateInfoTexts();

    void UpdateButtonLocations();

    void UpdatePlayerInfoText();

    glm::vec2 GetWindowSize() const;

    void PlayerJoinCallback(PlayerInputHandler& handler);

public:
    void Enter() override;
    void Exit() override;
    void Update() override;
    void Render() override;

    void SetErrorText(const std::string& text);
};

#endif //PROG4MINIGINPROJECT_MAINMENUSTATE_H