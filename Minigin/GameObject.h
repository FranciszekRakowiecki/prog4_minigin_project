#pragma once
#include <string>
#include <memory>

#include "Component.h"
#include "Transform.h"

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

		template<typename T, typename = std::enable_if<std::is_base_of_v<Component, T>>>
		Reference<T> AddComponent() {
			std::shared_ptr<T> component = std::make_shared<T>();
			m_Components.emplace_back(component);
			component->m_Parent = this;
			component->flags = component->GetFlags();

			component->Start();

			return Reference<T>(component);
		}

		template<typename T, typename = std::enable_if<std::is_base_of_v<Component, T>>>
		Reference<T> GetComponent() const {
			for (int index = 0; index < (int)GetComponentsCount(); ++index) {
				Reference<Component> component = GetComponent(index);
				if (component) {
					T* result = dynamic_cast<T*>(component);
					if (result != nullptr) {
						return Reference<T>(result);
					}
				}
			}
			return Reference<T>(nullptr);
		}

		Reference<Component> GetComponent(int index) const;
		size_t GetComponentsCount() const;

		friend class Scene;
	};
}
