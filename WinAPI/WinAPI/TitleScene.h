#pragma once
#include "Scene.h"
#include "pch.h"
#include "Graphics.h"

class ResourceManager;
class SceneManager;

class TitleScene : public Scene
{
public:
	TitleScene(ResourceManager* resourceManager,SceneManager* sceneManager);
	~TitleScene();

	void Init() override;

	void FixedUpdate() override;
	void Update(float deltaTime) override;
	void LateUpdate() override;

	void PreRender() override;
	void Render(ID2D1DeviceContext* context) override;
	void PostRender(ID2D1DeviceContext* context) override;

	void Release() override;

private:
	void RenderButton(const D2D1_RECT_F& rect,const wchar_t* text,bool hover);

private:
	ResourceManager* m_resourceManager;
	SceneManager* m_sceneManager;

	D2D1_RECT_F m_startButton;
	D2D1_RECT_F m_exitButton;
	D2D1_RECT_F m_loadButton;

	bool m_startHover;
	bool m_exitHover;
	bool m_loadHover;
};

