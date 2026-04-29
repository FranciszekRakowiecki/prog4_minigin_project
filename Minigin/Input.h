//
// Created by frane on 3/11/2026.
//
#pragma once

#ifndef MINIGIN_INPUT_H
#define MINIGIN_INPUT_H
#include <map>
#include <memory>
#include <vector>
#include <SDL3/SDL.h>

// Custom macros to fit into the button system
#ifndef __EMSCRIPTEN__
#define XINPUT_GAMEPAD_LEFT_TRIGGER 0x10000
#define XINPUT_GAMEPAD_RIGHT_TRIGGER 0x20000
#endif

namespace dae {
    class Minigin;
}

namespace dae {

    enum InputAxisType : uint8_t {
        SCROLL,
        MOUSE,
        CURSOR_POSITION,
        GAMEPAD_LEFT,
        GAMEPAD_RIGHT,
    };

    enum class GamepadButton {
        DPAD_UP = 0x0001,
        DPAD_DOWN = 0x0002,
        DPAD_LEFT = 0x0004,
        DPAD_RIGHT = 0x0008,
        START = 0x0010,
        BACK = 0x0020,
        LEFT_THUMB = 0x0040,
        RIGHT_THUMB = 0x0080,
        LEFT_SHOULDER = 0x0100,
        RIGHT_SHOULDER = 0x0200,
        A = 0x1000,
        B = 0x2000,
        X = 0x4000,
        Y = 0x8000,
        LEFT_TRIGGER = 0x10000,
        RIGHT_TRIGGER = 0x20000
    };

    struct InputCommand {
        virtual ~InputCommand() = default;

        virtual void frame() = 0;
        virtual void keyPress(int key) = 0;
        virtual void keyRelease(int key) = 0;

        virtual void buttonPress(int button) = 0;
        virtual void buttonRelease(int button) = 0;
    };

    struct InputKey : InputCommand {
        const int key;
        bool isPressed() const;
        bool pressedThisFrame() const;
        bool releasedThisFrame() const;
    private:
        bool framePress{};
        bool frameRelease{};
        InputKey(int key);

    public:
        void frame() override;

        void keyPress(int key) override;

        void keyRelease(int key) override;

        void buttonPress(int button) override;

        void buttonRelease(int button) override;

    private:
        friend class Input;
    };

    struct InputButton : InputCommand {
        const int button;
        bool isPressed() const;
        bool pressedThisFrame() const;
        bool releasedThisFrame() const;
    private:
        bool framePress{};
        bool frameRelease{};
        InputButton(int button);

    public:
        void frame() override;

        void keyPress(int key) override;

        void keyRelease(int key) override;

        void buttonPress(int button) override;

        void buttonRelease(int button) override;

    private:
        friend class Input;
    };

    struct InputGamepadButton : InputCommand {
    private:
        bool pressed{false};
        bool framePress{};
        bool frameRelease{};
        InputGamepadButton(GamepadButton button);

    public:

        bool isPressed() const;
        bool pressedThisFrame() const;
        bool releasedThisFrame() const;

        const GamepadButton button;

        void frame() override;

        void keyPress(int key) override;

        void keyRelease(int key) override;

        void buttonPress(int button) override;

        void buttonRelease(int button) override;

        friend class Input;
    };

    struct InputAxis {
        const InputAxisType axisType;

        float getX() const;
        float getY() const;

    private:
        float x{};
        float y{};

        explicit InputAxis(InputAxisType axis);

        friend class Input;
    };

    class Input {
    public:
        class GamepadImpl;
    private:

        static Input* Instance;

        Minigin* engine;

        InputAxis* MOUSE_DELTA{nullptr};
        InputAxis* CURSOR{nullptr};
        InputAxis* SCROLL_DELTA{nullptr};

        InputAxis* LEFT_THUMB_STICK{nullptr};
        InputAxis* RIGHT_THUMB_STICK{nullptr};

        SDL_Window* window{nullptr};

        std::unique_ptr<GamepadImpl> gamepadImpl{nullptr};

        std::map<int, InputKey*> keyActions{};
        std::map<int, InputButton*> buttonActions{};
        std::map<GamepadButton, InputGamepadButton*> gamepadButtonActions{};

        Input(Minigin* engine, SDL_Window* window);
        ~Input();

        void pollEvents();
        void pollKeys();
        void pollButtons();
        void pollAxis();

        void setPressed(InputGamepadButton* button, bool state);
        void setXY(InputAxis* axis, float x, float y);

        friend class Minigin;
        friend class GamepadImpl;

    public:
        InputKey const * getKey(int key);
        InputButton const * getButton(int button);
        InputGamepadButton const * getGamepadButton(GamepadButton button);

        InputAxis const * getMouseDelta();
        InputAxis const * getCursor();
        InputAxis const * getScrollDelta();

        int GetGamePad() const;

        /**
         *
         * @param key SDL_SCANCODE
         * @return
         */
        static const InputKey* KEY(int key);

        /**
         *
         * @param button SDL_BUTTON
         * @return /
         */
        static InputButton const * BUTTON(int button);
        static InputGamepadButton const * GAMEPAD_BUTTON(GamepadButton button);
        static InputAxis const * AXIS(InputAxisType type);

        class GamepadImpl;
    };
}


#endif //MINIGIN_INPUT_H