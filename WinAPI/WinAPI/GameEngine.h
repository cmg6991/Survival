#pragma once

#include "pch.h"
#include "GameObject.h"
#include "Graphics.h"

class SceneManager;
class ResourceManager;

class GameEngine
{
public:
	GameEngine();
	~GameEngine();

	void Init(const HWND hwnd);
	void Update(float deltaTime);
	void LateUpdate();
	void Render(ID2D1DeviceContext* context);
	void Release();
	//void CreateWall(float x, float y, const string& imageName);

private:
	ResourceManager* m_resourceManager;
	SceneManager* m_sceneManager;

	//vector<GameObject*> m_objects;
};

