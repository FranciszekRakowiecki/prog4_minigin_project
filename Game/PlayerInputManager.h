//
// Created by frane on 5/31/2026.
//

#ifndef PROG4MINIGINPROJECT_PLAYERINPUTMANAGER_H
#define PROG4MINIGINPROJECT_PLAYERINPUTMANAGER_H
#include <vector>

#include "Input.h"
#include "PlayerInputHandler.h"
#include "Singleton.h"


class PlayerInputManager : public dae::Singleton<PlayerInputManager> {
public:
    void initialize();
    void update();

    const std::vector<PlayerInputHandler>& getPlayers() const;

private:
    std::vector<PlayerInputHandler> m_Players{};
    PlayerInputManager();
    ~PlayerInputManager();

    dae::CommandCallbackId m_AnyKeyCallbackId{0};

    bool m_HasKeyPlayer0{false};
    bool m_HasKeyPlayer1{false};

    bool m_HasGamepadPlayer0{false};
    bool m_HasGamepadPlayer1{false};

    friend class dae::Singleton<PlayerInputManager>;
};


#endif //PROG4MINIGINPROJECT_PLAYERINPUTMANAGER_H
