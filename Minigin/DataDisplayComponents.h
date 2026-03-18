//
// Created by frane on 3/18/2026.
//

#ifndef MINIGIN_DATADISPLAYCOMPONENTS_H
#define MINIGIN_DATADISPLAYCOMPONENTS_H
#include "Component.h"
#include "Font.h"

namespace dae {
    class Text;
}

class LivesScoreRenderer final : public dae::Component {
    dae::GameObject* m_playerOne;
    dae::GameObject* m_playerTwo;
    dae::Font* m_font;

    dae::Text* m_livesOne;
    dae::Text* m_livesTwo;

    dae::Text* m_scoreOne;
    dae::Text* m_scoreTwo;
public:
    void setPlayers(dae::GameObject* playerOne, dae::GameObject* playerTwo);
    void setFont(dae::Font *font);
    void Render() override;
    void Update() override;
    int GetFlags() override;
};

#endif //MINIGIN_DATADISPLAYCOMPONENTS_H