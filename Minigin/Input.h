//
// Created by frane on 3/11/2026.
//
#pragma once

#ifndef MINIGIN_INPUT_H
#define MINIGIN_INPUT_H
#include <map>
#include <memory>
#include <vector>
#include <array>
#include <functional>
#include <SDL3/SDL.h>

// Custom macros to fit into the button system
#ifndef __EMSCRIPTEN__
#define XINPUT_GAMEPAD_LEFT_TRIGGER 0x10000
#define XINPUT_GAMEPAD_RIGHT_TRIGGER 0x20000
#endif

class GamepadImplSDL;

namespace dae {
    class Minigin;
}

namespace dae {

    enum class CommandType : uint8_t {
        KEY_PRESS, KEY_RELEASE, BUTTON_PRESS, BUTTON_RELEASE, GAMEPAD_BUTTON_PRESS, GAMEPAD_BUTTON_RELEASE, AXIS_MOVE, NONE
    };

    enum class InputAxisType : uint8_t {
        SCROLL,
        MOUSE_DELTA,
        CURSOR_POSITION,
        GAMEPAD_LEFT,
        GAMEPAD_RIGHT,
        NONE
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
        RIGHT_TRIGGER = 0x20000,
        NONE = 0xFFFFFF
    };

    struct CommandContext {
        CommandType type{CommandType::NONE};
        InputAxisType axis{InputAxisType::NONE};
        uint8_t button{0};
        GamepadButton gamepadButton{GamepadButton::NONE};
        SDL_Scancode scanCode{SDL_SCANCODE_0};
        float axisX{0.0f}, axisY{0.0f};
        uint32_t gamepadIndex{0};
    };

    using CommandCallbackId = uint32_t;
    using CommandCallbackFunc = std::function<void(const CommandContext&)>;

    struct CommandCallback {
        CommandCallbackId id{0};
        CommandCallbackFunc callback;
    };

    using CommandCallbacks = std::vector<CommandCallback>;

    // This approach is mid
    // You have to keep track of the input yourself and there is no easy ish way to automatically unbind the callback when an object is deleted.
    // It makes the client prone to forgetting to unbind the call.
    // Also it makes it rather difficult to find references to specific scene objects if it is instead meant to use the virtual void execute format instead of the std::function
    // The awesome functionality of std::function is that i can pass in a reference to an object and within the function it will still have access to private members
    // making this approach slightly less how do i put it... shit.
    // And even then organizing your code with IsPressed, WasPressedThisFrame can happen just structuring checks in their own independent contexts correctly such that you dont have a huge if else going for the entire update function...
    struct InputCommand {
        CommandCallbacks onPerformed{};
        void execute(const CommandContext&);
        CommandCallbackId addListener(CommandCallbackFunc cb);

        /**
         * Cannot be executed during an onPerformed call
         * @param idx Stored id of the callback
         */
        void removeListener(CommandCallbackId idx);

    private:
        uint32_t callbackIndex{0};
    };

    struct InputAction : public InputCommand {
        virtual ~InputAction() = default;

        virtual void frame() = 0;
        virtual void keyPress(int key) = 0;
        virtual void keyRelease(int key) = 0;

        virtual void buttonPress(int button) = 0;
        virtual void buttonRelease(int button) = 0;
    };

    struct InputKey : InputAction {
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

    struct InputButton : InputAction {
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

    struct InputGamepadButton : InputAction {
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

    struct InputAxis : public InputCommand {
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
        static constexpr int MaxGamepads{4};
    private:
        static Input* Instance;

        Minigin* engine;

        InputAxis* MOUSE_DELTA{nullptr};
        InputAxis* CURSOR{nullptr};
        InputAxis* SCROLL_DELTA{nullptr};

        std::array<InputAxis*, MaxGamepads> LEFT_THUMB_STICKS{};
        std::array<InputAxis*, MaxGamepads> RIGHT_THUMB_STICKS{};

        SDL_Window* window{nullptr};

        std::unique_ptr<GamepadImpl> gamepadImpl{nullptr};

        std::map<int, InputKey*> keyActions{};
        std::map<int, InputButton*> buttonActions{};
        std::array<std::map<GamepadButton, InputGamepadButton*>, MaxGamepads> gamepadButtonActions{};

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
        friend class ::GamepadImplSDL;

    public:
        InputKey const * getKey(int key);
        InputButton const * getButton(int button);
        InputGamepadButton const * getGamepadButton(GamepadButton button);
        InputGamepadButton const * getGamepadButton(int gamepadIndex, GamepadButton button);

        InputAxis const * getMouseDelta();
        InputAxis const * getCursor();
        InputAxis const * getScrollDelta();

        int GetGamePad(int gamepadIndex = 0) const;

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
        static InputGamepadButton const * GAMEPAD_BUTTON(int gamepadIndex, GamepadButton button);
        static InputAxis const * AXIS(InputAxisType type);
        static InputAxis const * AXIS(int gamepadIndex, InputAxisType type);
    };
}


#endif //MINIGIN_INPUT_H
