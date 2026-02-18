#include <string>
#include "GameObject.h"

#include <iostream>

#include "ResourceManager.h"
#include "Renderer.h"
#include "Component.h"

dae::GameObject::~GameObject() {
	for (int index = 0; index < (int)m_Components.size(); ++index) {
		m_Components[index]->Destroy();
	}
	m_Components.clear();
}

void dae::GameObject::update_internal() {
	for (int index = m_Components.size() - 1; index >= 0; --index) {
		if (!m_Components[index]->m_IsDisposed)
			continue;

		if (index == (int)m_Components.size() - 1)
			m_Components.pop_back();
		else {
			m_Components[index] = m_Components[m_Components.size() - 1];
			m_Components.pop_back(); // i'm unsure now if it skips a check for disposal here or if there is a chance for it but doesn't actually matter that much
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
}

void dae::GameObject::Update() {


}

void dae::GameObject::Render() const
{
	const auto& pos = m_transform.GetPosition();
	Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
}

void dae::GameObject::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::GameObject::SetPosition(float x, float y)
{
	m_transform.SetPosition(x, y, 0.0f);
}

std::weak_ptr<dae::Component> dae::GameObject::GetComponent(int index) const {
	return m_Components[index];
}

size_t dae::GameObject::GetComponentsCount() const {
	return m_Components.size();
}

