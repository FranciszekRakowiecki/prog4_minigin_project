//
// Created by frane on 2/18/2026.
//
#pragma once
#ifndef MINIGIN_COMPONENT_H
#define MINIGIN_COMPONENT_H
#include <memory>
#include <cassert>

#include "SoundSystem.h"
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
        Reference();
        Reference(std::weak_ptr<T> ptr);
        Reference(const Reference&);
        Reference& operator=(const Reference&);
        Reference(Reference&&);
        Reference& operator=(Reference&&);

        T* operator->() const;
        T& operator*() const;
        T* get() const { if (*this) return  m_Ptr.lock().get(); return nullptr; };
        std::weak_ptr<T> getWeak() const { return m_Ptr; }
        template<typename any>
        bool operator==(const Reference<any> &other) const {
            return other.get() == m_Ptr.lock().get();
        }

        explicit operator bool() const { return !m_Ptr.expired(); }

    private:
        std::weak_ptr<T> m_Ptr;
    };

    template<typename to, typename from>
    inline Reference<to> StaticCastReference(Reference<from> &other) {
        return Reference<to>(std::static_pointer_cast<to>(other.getWeak().lock()));
    }
    template<typename to, typename from>
    inline Reference<to> DynamicCastReference(Reference<from> &other) {
        return Reference<to>(std::dynamic_pointer_cast<to>(other.getWeak().lock()));
    }

    template<typename T>
    Reference<T>::Reference() : m_Ptr() {

    }

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
    Reference<T>::Reference(Reference &&other) {
        m_Ptr = std::move(other.m_Ptr);
    }

    template<typename T>
    Reference<T> & Reference<T>::operator=(Reference &&other) {
        m_Ptr = std::move(other.m_Ptr);
        return *this;
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

    // Tried to get interfaces to work but it just wont give
}

#endif //MINIGIN_COMPONENT_H