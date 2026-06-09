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

        virtual const glm::vec2& GetMove() = 0;
        virtual void Update() = 0;
    };

    struct ShootInput {
        virtual ~ShootInput() = default;

        ShootInput(uint32_t, uint32_t) {};

        virtual bool IsShooting() const = 0;
        virtual void Update() = 0;
    };

    PlayerInputHandler(uint32_t gamepadIndex, uint32_t playerIndex, bool isGamepad);
    void update();

    const glm::vec2& GetMove() const { return m_Move; }
    bool IsShooting() const { return m_IsShooting; }
    uint32_t GetGamepadIndex() const { return m_GamepadIndex; }
    uint32_t GetPlayerIndex() const { return m_PlayerIndex; }
    bool IsGamepad() const { return m_IsGamepad; }

    float GetStartTime() const;

private:
    bool m_IsShooting{false};
    bool m_IsGamepad{false};
    glm::vec2 m_Move{0.0f};
    uint32_t m_GamepadIndex;
    uint32_t m_PlayerIndex;
    float m_StartTime;

    std::unique_ptr<MoveInput> m_MoveInput;
    std::unique_ptr<ShootInput> m_ShootInput;
};


#endif //PROG4MINIGINPROJECT_PLAYERINPUTHANDLER_H