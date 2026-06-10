//
// Created by frane on 6/9/2026.
//

#ifndef PROG4MINIGINPROJECT_TANKCONTROLLER_H
#define PROG4MINIGINPROJECT_TANKCONTROLLER_H
#include "Component.h"


class PlayerInputHandler;

class TankController : public dae::Component {
public:
    int GetFlags() override;
    void Update() override;

    void SetPlayerInput(const PlayerInputHandler* handler);

private:
    const PlayerInputHandler* m_PlayerInput;
};


#endif //PROG4MINIGINPROJECT_TANKCONTROLLER_H