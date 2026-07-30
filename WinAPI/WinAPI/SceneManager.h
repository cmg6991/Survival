#pragma once
#include "pch.h" 
#include "Graphics.h"

class Scene;
class ResourceManager;
class GameProcess;

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void Init(ResourceManager* resourceManager);

	void FixedUpdate();
	void Update(float deltaTime);
	void LateUpdate();

	void PreRender();
	void Render(ID2D1DeviceContext* context);
	void PostRender(ID2D1DeviceContext* context);

	void Release();

	void SetNextScene(string sceneName) { m_nextScene = m_scenes[sceneName]; }
	void LoadScene(string sceneName);
	void CreateScene(string sceneName, std::function<Scene*()> factory);
	Scene* FindScene(string sceneName);

	Scene* GetCurrentScene() { return m_currentScene; }
	Scene* GetPreScene() { return m_preScene; }
	void	SetCurrentScene(std::string sceneName) { m_currentScene = m_scenes[sceneName]; }

	ResourceManager* GetResourceManager() { return m_resourceManager; }
private:
	Scene* m_currentScene;
	Scene* m_nextScene;
	Scene* m_preScene;
	unordered_map<string, Scene*> m_scenes;
	//unordered_map<string, function<Scene*()>> m_sceneInitFunctions;

	ResourceManager* m_resourceManager;
	
	int m_loadCount = 0;
};

