#pragma once
#include <glm/glm.hpp>

namespace dae {
	class GameObject;
}

namespace dae
{
	class Transform final
	{
	public:
		const glm::vec3& GetPosition() const;
		const glm::vec3& GetWorldPosition() const;

		void SetLocalPosition(float x, float y, float z = 0);
		void SetLocalPosition(const glm::vec3& position);
		void SetWorldPosition(const glm::vec3& position);
		void SetWorldPosition(float x, float y, float z = 0);

		void SetRotation(float x);
		float GetRotation() const;

		explicit Transform(GameObject* parent);

	private:
		friend class GameObject;

		glm::vec3 m_position{};
		mutable glm::vec3 m_worldPosition{};
		float m_rotation{};
		mutable bool m_isWorldPositionDirty{ true };

		GameObject* m_parent{};

		void MarkWorldPositionDirty();
		void UpdateWorldPosition() const;
		glm::vec3 GetParentWorldPosition() const;
	};
}
