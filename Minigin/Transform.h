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

		void SetDirty();

		explicit Transform(GameObject* parent);

	private:
		glm::vec3 m_position{};
		glm::vec3 m_worldPosition{};
		float m_rotation{};

		GameObject* m_parent;

		// Returning without reference because there might be no parent
		glm::vec3 GetParentWorldPosition() const;
	};
}
