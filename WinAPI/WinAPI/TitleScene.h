#pragma once
#include "Scene.h"
#include "pch.h"
#include "Graphics.h"
#include "SaveData.h"

class ResourceManager;
class SceneManager;

enum class TitleMenuState
{
	Main,
	NewGameSlot,
	LoadGameSlot
};

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
	void SelectSlot(int slot);

	void RenderSlotButton(const D2D1_RECT_F& rect,int slot,bool hover);
	void RenderSlotPanel();
	void RenderSlotTitle();
	void RenderConfirmWindow();

	bool IsSlotOccupied(int slot);

private:
	ResourceManager* m_resourceManager; 
	SceneManager* m_sceneManager;

	D2D1_RECT_F m_startButton;
	D2D1_RECT_F m_exitButton;
	D2D1_RECT_F m_loadButton;
	D2D1_RECT_F m_backButton;

	bool m_startHover;
	bool m_exitHover;
	bool m_loadHover;
	bool m_backHover;

	// 슬롯 메뉴
	D2D1_RECT_F m_slotPanel;
	D2D1_RECT_F m_slotButtons[3];

	bool m_slotHover[3];

	// 현재 타이틀 메뉴
	TitleMenuState m_menuState;


	bool m_showConfirm = false;

	int m_selectedSlot = 0;

	SaveData m_slotData[3];

	bool m_slotExists[3] = { false,false,false };
};

