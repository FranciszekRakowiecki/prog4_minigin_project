#include "Transform.h"

#include "GameObject.h"

const glm::vec3 & dae::Transform::GetPosition() const {
	return m_position;
}

const glm::vec3 & dae::Transform::GetWorldPosition() const {
	return m_worldPosition;
}

void dae::Transform::SetWorldPosition(float x, float y, float z) {
	SetWorldPosition({ x, y, z});
}

void dae::Transform::SetRotation(float x) {
	m_rotation = x;
}

void dae::Transform::SetLocalPosition(const float x, const float y, const float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	SetDirty();
}

void dae::Transform::SetLocalPosition(const glm::vec3& position)
{ 
	m_position = position;
	SetDirty();
}

// Wouldn't it be better if the world position for all objects was recalculated at once instead of having to wait for an update, it could cause inaccurate results if there are a lot of world position assignments.
void dae::Transform::SetWorldPosition(const glm::vec3 &position) {
	m_worldPosition = position;

	m_position = m_worldPosition - GetParentWorldPosition();

	SetDirty();
}

float dae::Transform::GetRotation() const {
	return m_rotation;
}

void dae::Transform::SetDirty() {
	m_worldPosition = GetParentWorldPosition() + m_position;
	m_parent->TransformIsDirty();
}

dae::Transform::Transform(GameObject *parent) : m_parent(parent) {
	assert(parent != nullptr);
	SetDirty();
}

glm::vec3 dae::Transform::GetParentWorldPosition() const {
	if (m_parent->GetParent() != nullptr)
		return m_parent->GetParent()->transform.GetWorldPosition();
	return {};
}
