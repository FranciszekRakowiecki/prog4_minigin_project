#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Update()
{
	for(auto& scene : m_scenes)
	{
		m_ActiveScene = scene.get();
		scene->Update();
	}
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_scenes)
	{
		scene->Render();
	}
}

dae::Scene * dae::SceneManager::GetActiveScene() {
	return m_ActiveScene;
}

void dae::SceneManager::UnloadScene(Scene *scene) {
	std::erase_if(m_scenes, [&](const auto& other) { return other.get() == scene; });
	if (m_ActiveScene == scene) {
		m_ActiveScene = nullptr;
	}
}

dae::Scene* dae::SceneManager::CreateScene()
{
	m_scenes.emplace_back(new Scene());
	m_ActiveScene = m_scenes.back().get();
	return m_scenes.back().get();
}
