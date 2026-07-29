#pragma once

#include <windows.h>

class WinInit;
class GameEngine;

class GameProcess
{
public:
	GameProcess();
	~GameProcess();

public:
	void Init(HINSTANCE hInstance);
	void Loop();
	void Release();

	void FixedUpdate();
	void Update();
	void LateUpdate();

	void PreRender();
	void Render();
	void PostRender();

private:
	HWND m_hWnd;
	WinInit* m_winInit;
	//HDC m_hdc;
	//HBITMAP m_hBitmap;

	float m_x;
	float m_y;

	GameEngine* m_gameEngine;
};