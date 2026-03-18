//
// Created by frane on 2/18/2026.
//
#pragma once
#ifndef MINIGIN_COMPONENT_H
#define MINIGIN_COMPONENT_H
#include <memory>
#include <cassert>

#include "glm/vec3.hpp"

namespace dae {
    struct InputGamepadButton;
}

namespace dae {
    struct InputKey;
}

namespace dae {
    class GameObject;

#define COMPONENT_HAS_UPDATE 1
#define COMPONENT_HAS_RENDER 2
    // Add more

    // Components are not allowed to have constructor arguments
    class Component {

    public:
        Component() = default;

        virtual ~Component() = default;
        Component& operator=(const Component&) = delete;
        Component(const Component&) = delete;
        Component(Component&&) = delete;
        Component& operator=(Component&&) = delete;

        void Destroy();

        bool IsDestroyed() const { return m_IsDisposed; }

        GameObject* GetParent() const;

        virtual int GetFlags(); // Checking using bit masks is faster than doing dynamic_cast<>
        virtual void Start();
        virtual void Update();
        virtual void Render();

    private:
        GameObject* m_Parent{};

        int flags;

        bool m_IsDisposed{0}; // Marked for removal
        // Also use this to cancel the execution of functions
        // as to not use any resources from a gameobject if a component reference is stored elsewhere

        friend class GameObject;
        // I know using friend is unfavorable but components should never be allowed to be initialized without a game object and
        // this allows for easily setting the parent without having to relly that the user implements a valid constructor that sets the private member.
    };

    // Allowing components to instead be passed around using this
    template<typename T>
    class Reference final {
    public:
        Reference(std::weak_ptr<T> ptr);
        Reference(const Reference&);
        Reference& operator=(const Reference&);
        Reference(Reference&&) = delete;
        Reference& operator=(Reference&&) = delete;

        T* operator->() const;
        T& operator*() const;

        explicit operator bool() const { return !m_Ptr.expired(); }

    private:
        std::weak_ptr<T> m_Ptr;
    };

    template<typename T>
    dae::Reference<T>::Reference(std::weak_ptr<T> ptr) : m_Ptr(ptr) {

    }

    template<typename T>
    dae::Reference<T>::Reference(const Reference & other) {
        this->m_Ptr = other.m_Ptr;
    }

    template<typename T>
    dae::Reference<T> & dae::Reference<T>::operator=(const Reference & other) {
        this->m_Ptr = other.m_Ptr;
    }

    template<typename T>
    T * dae::Reference<T>::operator->() const {
        assert(!m_Ptr.expired());
        return m_Ptr.lock().get();
    }

    template<typename T>
    T & dae::Reference<T>::operator*() const {
        assert(!m_Ptr.expired());
        return *m_Ptr.lock().get();
    }

    class ExampleUpdateComponent : public Component {
    public:
        int GetFlags() override;
        void Update() override;
    };

    class ExampleRotator : public Component {
        public:
        ExampleRotator();
        float rotatorSpeed{1.0f};
        int GetFlags() override;
        void Update() override;
        void Start() override;
    private:
        glm::vec3 offset;
    };

    class ExampleMovementKeyboard : public Component {
        const InputKey* forward;
        const InputKey* back;
        const InputKey* left;
        const InputKey* right;

        public:
            ExampleMovementKeyboard();

            void Update() override;
            int GetFlags() override;

    };

    class ExampleMovementDPAD : public Component {
        const InputGamepadButton* forward;
        const InputGamepadButton* back;
        const InputGamepadButton* left;
        const InputGamepadButton* right;

    public:
        ExampleMovementDPAD();

        void Update() override;
        int GetFlags() override;

    };

    // Tried to get interfaces to work but it just wont give
}

#endif //MINIGIN_COMPONENT_H