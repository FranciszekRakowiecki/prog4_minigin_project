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
		std::shared_ptr<Texture2D> m_texture{};

		std::vector<std::shared_ptr<Component>> m_Components{};

		std::vector<std::unique_ptr<GameObject>> m_Children{};

		void update_internal();
		void render_internal() const;

		GameObject* removeChild(GameObject *child);
		void addChild(std::unique_ptr<GameObject> child);

		// Used specifically to update children
		void TransformIsDirty();

		GameObject* m_Parent{nullptr};
		bool m_IsDisposed{};

	public:

		Transform transform;

		virtual void Update();
		virtual void Render() const;

		void SetTexture(const std::string& filename);
		void SetPosition(float x, float y);

		GameObject();
		virtual ~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		void Destroy();

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

		GameObject* GetParent() const;

		void SetParent(GameObject* parent);

		Reference<Component> GetComponent(int index) const;
		size_t GetComponentsCount() const;

		friend class Scene;
		friend class Transform;
	};
}
