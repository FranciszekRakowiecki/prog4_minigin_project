#include "Transform.h"

#include <cassert>

#include "GameObject.h"

const glm::vec3 & dae::Transform::GetPosition() const {
	return m_position;
}

const glm::vec3 & dae::Transform::GetWorldPosition() const {
	if (m_isWorldPositionDirty)
		UpdateWorldPosition();

	return m_worldPosition;
}

void dae::Transform::SetWorldPosition(float x, float y, float z) {
	SetWorldPosition({ x, y, z });
}

void dae::Transform::SetRotation(float x) {
	m_rotation = x;
}

void dae::Transform::SetLocalPosition(const float x, const float y, const float z) {
	SetLocalPosition({ x, y, z });
}

void dae::Transform::SetLocalPosition(const glm::vec3& position) {
	if (m_position == position)
		return;

	m_position = position;
	MarkWorldPositionDirty();
}

void dae::Transform::SetWorldPosition(const glm::vec3 &position) {
	const glm::vec3 localPosition = position - GetParentWorldPosition();
	SetLocalPosition(localPosition);
}

float dae::Transform::GetRotation() const {
	return m_rotation;
}

dae::Transform::Transform(GameObject *parent) : m_parent(parent) {
	assert(parent != nullptr);
}

void dae::Transform::MarkWorldPositionDirty() {
	m_isWorldPositionDirty = true;
	m_parent->TransformIsDirty();
}

void dae::Transform::UpdateWorldPosition() const {
	m_worldPosition = GetParentWorldPosition() + m_position;
	m_isWorldPositionDirty = false;
}

glm::vec3 dae::Transform::GetParentWorldPosition() const {
	if (m_parent->GetParent() != nullptr)
		return m_parent->GetParent()->transform.GetWorldPosition();
	return {};
}
