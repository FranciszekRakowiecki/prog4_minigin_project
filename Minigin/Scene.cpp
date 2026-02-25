#include <algorithm>
#include "Scene.h"

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_objects.emplace_back(std::move(object));
}

void Scene::Remove(GameObject& object)
{
	object.Destroy();
}

void Scene::RemoveAll()
{
	m_objects.clear();
}

void Scene::Update()
{
	// Removing all gameobjects marked for disposal
	for (int index = (int)m_objects.size() - 1; index >= 0; --index) {
		if (!m_objects[index]->m_IsDisposed)
			continue;

		if (index == (int)m_objects.size() - 1)
			m_objects.pop_back();
		else {
			m_objects[index] = std::move(m_objects[m_objects.size() - 1]);
			m_objects.pop_back();
		}
	}

	int size = (int)m_objects.size();
	for(int index = 0; index < size; ++index)
	{
		m_objects[index]->update_internal();
		size = (int)m_objects.size();// Temporary fix because set parent requires its own vector and functions before the update call to safely transpose it to a new parent.
		// This should? hopefully make it so that when looping there is less of an issue to encounter a missing game object.
	}
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		object->render_internal();
	}
}

GameObject * Scene::pop(GameObject *object) {
	for (int index = 0; index < (int)m_objects.size(); ++index) {
		if (m_objects[index].get() == object) {
			// TODO: Revise a safe deletion method for this
			GameObject* tmp = m_objects[index].release();
			m_objects.erase(m_objects.begin() + index);
			return tmp;
		}
	}
	return nullptr;
}

