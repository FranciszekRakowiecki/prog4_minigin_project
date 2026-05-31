//
// Created by frane on 5/29/2026.
//

#ifndef PROG4MINIGINPROJECT_PLAYERINPUTHANDLER_H
#define PROG4MINIGINPROJECT_PLAYERINPUTHANDLER_H
#include <memory>
#include <glm/vec2.hpp>


class PlayerInputHandler {
public:
    struct MoveInput {
        MoveInput(uint32_t, uint32_t) {};
        virtual ~MoveInput() = default;

        virtual const glm::vec2& getMove() = 0;
        virtual void update() = 0;
    };

    struct ShootInput {
        virtual ~ShootInput() = default;

        ShootInput(uint32_t, uint32_t) {};

        virtual bool isShooting() const = 0;
        virtual void update() = 0;
    };

    PlayerInputHandler(uint32_t gamepadIndex, uint32_t playerIndex, bool isGamepad);
    void update();

private:
    bool m_IsShooting{false};
    bool m_IsGamepad{false};
    glm::vec2 m_Move{0.0f};
    uint32_t m_GamepadIndex;

    std::unique_ptr<MoveInput> m_MoveInput;
    std::unique_ptr<ShootInput> m_ShootInput;
};


#endif //PROG4MINIGINPROJECT_PLAYERINPUTHANDLER_H