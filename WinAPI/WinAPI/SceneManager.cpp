#include "SceneManager.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "Graphics.h"

SceneManager::SceneManager()
	:m_currentScene(nullptr), m_nextScene(nullptr), m_resourceManager(nullptr), m_scenes(), m_preScene(nullptr), m_loadCount(0)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init(ResourceManager* resourceManager)
{
	m_resourceManager = resourceManager;
}

void SceneManager::FixedUpdate()
{
	m_currentScene->FixedUpdate();
}
void SceneManager::Update(float deltaTime)
{
	m_currentScene->Update(deltaTime);
}

void SceneManager::LateUpdate()
{
	m_currentScene->LateUpdate();
	if (m_nextScene != nullptr)
	{
		LoadScene(m_nextScene->GetName());
	}
}

void SceneManager::PreRender()
{
	m_currentScene->PreRender();
}

void SceneManager::Render(ID2D1DeviceContext* context)
{
	m_currentScene->Render(context);
}

void SceneManager::PostRender(ID2D1DeviceContext* context)
{
	m_currentScene->PostRender(context);
}

void SceneManager::Release()
{
	for (auto& scene : m_scenes)
	{
		scene.second->Release();
		delete scene.second;
	}
	m_scenes.clear();

	m_currentScene = nullptr;
	m_nextScene = nullptr;
	m_preScene = nullptr;
}

void SceneManager::LoadScene(std::string sceneName)
{
	wchar_t buf[128];
	swprintf_s(buf, L"[LoadScene] called with: %S (loadCount=%d)\n", sceneName.c_str(), m_loadCount);
	OutputDebugStringW(buf);
	if (m_loadCount > 0)
	{
		m_currentScene->DeleteAllObject();
		//m_resourceManager->Release();
	}

	m_preScene = m_currentScene;
	m_currentScene = m_scenes[sceneName];
	m_currentScene->Init(); // 자식 클래스(MainScene 등)의 Init()이 다형성으로 호출됨
	m_nextScene = nullptr;
	m_loadCount++;
}

void SceneManager::CreateScene(std::string sceneName, std::function<Scene*()> factory)
{
	Scene* newScene = factory(); // 예: new MainScene(m_resourceManager)
	m_scenes[sceneName] = newScene;
}

Scene* SceneManager::FindScene(std::string sceneName)
{
	auto it = m_scenes.find(sceneName);
	if (it == m_scenes.end())
		return nullptr;
	return it->second;
}