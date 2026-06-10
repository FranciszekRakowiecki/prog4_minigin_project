#include <string>
#include "GameObject.h"

#include <iostream>

#include "ResourceManager.h"
#include "Renderer.h"
#include "Component.h"
#include "SceneManager.h"

dae::GameObject::~GameObject() {
	for (int index = 0; index < (int)m_Components.size(); ++index) {
		m_Components[index]->Destroy(); // Call in events in the future
	}
	m_Components.clear();

	m_Children.clear();
}

void dae::GameObject::update_internal() {
	for (int index = (int)m_Components.size() - 1; index >= 0; --index) {
		if (!m_Components[index]->m_IsDisposed)
			continue;

		if (index == (int)m_Components.size() - 1)
			m_Components.pop_back();
		else {
			m_Components[index] = m_Components[m_Components.size() - 1];
			m_Components.pop_back(); // i'm unsure now if it skips a check for disposal here or if there is a chance for it but doesn't actually matter that much
		}
	}

	for (int index = (int)m_Children.size() - 1; index >= 0; --index) { // This could be a little bit problematic because of possible behavior requiring children in a specific order but genuinely cba
		if (!m_Children[index]->m_IsDisposed)
			continue;

		if (index == (int)m_Children.size() - 1)
			m_Children.pop_back();
		else {
			m_Children[index] = std::move(m_Children[m_Children.size() - 1]);
			m_Children.pop_back();
		}
	}

	// TODO: Remove the TextObject GameObject inheritance and make GameObjects final.
	for (int index = 0; index < (int)m_Components.size(); ++index) {
		int flags = m_Components[index].get()->flags;
		if (flags & COMPONENT_HAS_UPDATE) { // TODO: Also would probably be good to put these into separate std::vectors for less iterations
			Component* component = m_Components[index].get();

			component->Update();
		}
	}

	Update();

	int size = (int)m_Children.size();
	for (int index = 0; index < size; ++index) {
		m_Children[index]->update_internal();
		size = (int)m_Children.size(); // Temporary fix because set parent requires its own vector and functions before the update call to safely transpose it to a new parent.
		// This should? hopefully make it so that when looping there is less of an issue to encounter a missing game object.
	}
}

void dae::GameObject::render_internal() const {
	Render();
	for (const std::shared_ptr<Component>& component : m_Components) {
		if (component->flags & COMPONENT_HAS_RENDER)
			component->Render();
	}

	for (const auto & index : m_Children) {
		index->render_internal();
	}
}

dae::GameObject* dae::GameObject::removeChild(GameObject *child) {
	for (int index = 0; index < (int)m_Children.size(); ++index) {
		if (m_Children[index].get() == child) {
			// TODO: Revise a safe deletion method for this
			GameObject* tmp = m_Children[index].release();
			m_Children.erase(m_Children.begin() + index);
			return tmp;
		}
	}
	return nullptr;
}

void dae::GameObject::addChild(std::unique_ptr<GameObject> child) {
	m_Children.push_back(std::move(child));
}

void dae::GameObject::TransformIsDirty() {
	for (int index = 0; index < (int)m_Children.size(); ++index) {
		m_Children[index]->transform.MarkWorldPositionDirty();
	}
}

void dae::GameObject::Update() {

}

void dae::GameObject::Render() const
{
}

void dae::GameObject::SetPosition(float x, float y)
{
	transform.SetLocalPosition(x, y, 0.0f);
}

dae::GameObject::GameObject() : transform(this) {
}

void dae::GameObject::Destroy() {
	m_IsDisposed = true;
}

dae::GameObject * dae::GameObject::GetParent() const {
	return m_Parent;
}

void dae::GameObject::SetParent(GameObject* parent) {
	if (parent == m_Parent)
		return;

	if (m_Parent && parent == nullptr) {
		m_Parent->removeChild(this);
		SceneManager::GetInstance().GetActiveScene()->Add(std::unique_ptr<GameObject>(this));
		m_Parent = nullptr;
		transform.MarkWorldPositionDirty();
		return;
	}

	if (m_Parent) {
		m_Parent->removeChild(this);
	}
	else {
		SceneManager::GetInstance().GetActiveScene()->pop(this);
	}
	m_Parent = parent;
	transform.MarkWorldPositionDirty();

	if (m_Parent)
		m_Parent->addChild(std::unique_ptr<GameObject>(this));
	else
		SceneManager::GetInstance().GetActiveScene()->Add(std::unique_ptr<GameObject>(this)); // This should be impossible but it keeps bitching about m_Parent having the possibility of still being nullptr.
}

dae::Reference<dae::Component> dae::GameObject::GetComponent(int index) const {
	return Reference<Component>(m_Components[index]);
}

size_t dae::GameObject::GetComponentsCount() const {
	return m_Components.size();
}

