#pragma once
#include <string>
#include <memory>
#include "Transform.h"

namespace dae {
	class Component;
}

namespace dae
{
	class Texture2D;
	class GameObject 
	{
		Transform m_transform{};
		std::shared_ptr<Texture2D> m_texture{};

		std::vector<std::shared_ptr<Component>> m_Components{};

		void update_internal();

	public:
		virtual void Update();
		virtual void Render() const;

		void SetTexture(const std::string& filename);
		void SetPosition(float x, float y);

		GameObject() = default;
		virtual ~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		// Trying to mirror 1 to 1 with how unity does it ish

		template<class T = Component>
		std::weak_ptr<T> AddComponent() {
			std::shared_ptr<T> component = std::make_shared<T>();
			m_Components.emplace_back(component);
			component->m_Parent = this;
			component->flags = component->GetFlags();

			return component;
		}

		template<typename T = Component>
		std::weak_ptr<T> GetComponent() const {
			for (int index = 0; index < (int)GetComponentsCount(); ++index) {
				std::weak_ptr<Component> component = GetComponent(index);
				if (!component.expired()) {
					std::shared_ptr<Component> _sharedcomponent = component.lock();
					T* result = dynamic_cast<T*>(_sharedcomponent.get());
					if (result != nullptr) {
						return _sharedcomponent;
					}
				}
			}
			return nullptr;
		}

		std::weak_ptr<Component> GetComponent(int index) const;
		size_t GetComponentsCount() const;

		friend class Scene;
	};
}
