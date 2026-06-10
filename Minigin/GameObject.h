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

		void SetPosition(float x, float y);

		GameObject();
		virtual ~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		void Destroy();
		bool IsDestroyed() const { return m_IsDisposed; }

		// Trying to mirror 1 to 1 with how unity does it ish

		template<typename T = Component>
		Reference<T> AddComponent() {
			std::shared_ptr<T> component = std::make_shared<T>();
			component->m_Parent = this;
			component->flags = component->GetFlags();
			m_Components.emplace_back(component);

			component->Start();

			return Reference<T>(component);
		}

		template<typename T>
		Reference<T> GetComponent() const {
			for (int index = 0; index < int(GetComponentsCount()); ++index) {
				Reference component{GetComponent(index)};
				T* result = dynamic_cast<T*>(component.get());
				if (result != nullptr) {
					return StaticCastReference<T>(component);
				}
			}
			return Reference<T>();
		}

		GameObject* GetParent() const;

		void SetParent(GameObject* parent);

		Reference<Component> GetComponent(int index) const;
		size_t GetComponentsCount() const;

		friend class Scene;
		friend class Transform;
	};
}
