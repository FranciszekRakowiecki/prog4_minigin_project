//
// Created by frane on 2/18/2026.
//
#pragma once
#ifndef MINIGIN_COMPONENT_H
#define MINIGIN_COMPONENT_H

namespace dae {
    class GameObject;

#define COMPONENT_HAS_UPDATE 1
    // Add more

    // Components are not allowed to have constructor arguments
    class Component {

    public:
        Component() = default;

        virtual ~Component() = default;

        void Destroy();

        bool IsDestroyed() const { return m_IsDisposed; }

        GameObject* GetParent() const;

        virtual int GetFlags(); // Checking using bit masks is faster than doing dynamic_cast<>
        virtual void Update();

    private:
        GameObject* m_Parent{};

        int flags;

        bool m_IsDisposed{0}; // Marked for removal
        // Also use this to cancel the execution of functions
        // as to not use any resources from a gameobject if a component reference is stored elsewhere

        friend class GameObject;
    };

    class ExampleUpdateComponent : public Component {
    public:
        int GetFlags() override;
        void Update() override;
    };

    // Tried to get interfaces to work but it just wont give
}

#endif //MINIGIN_COMPONENT_H