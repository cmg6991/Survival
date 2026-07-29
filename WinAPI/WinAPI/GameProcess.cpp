#include "GameProcess.h"
#include "WinInit.h"
#include "CameraManager.h"
#include "GameEngine.h"
#include "Graphics.h"

GameProcess::GameProcess() 
	: m_hWnd(NULL), m_winInit(nullptr),m_x(500),m_y(500),m_gameEngine(nullptr)/*m_hdc(NULL),m_hBitmap(NULL)*/
{

}

GameProcess::~GameProcess()
{
}

void GameProcess::Init(HINSTANCE hInstance)
{
    //ShowCursor(false);
    m_winInit = new WinInit;
    m_hWnd = m_winInit->Init(hInstance);

	/*HDC hdc = GetDC(m_hWnd);
	m_hdc = CreateCompatibleDC(hdc);
	m_hBitmap = CreateCompatibleBitmap(hdc, 1920, 1080);
	SelectObject(m_hdc, m_hBitmap);
	ReleaseDC(m_hWnd, hdc);*/

	if (!GRAPHICS.Init(m_hWnd))
	{
		// 초기화 실패 처리 (로그, 메시지박스 등)
		MessageBox(m_hWnd, L"Graphics Init Failed", L"Error", MB_OK);
	}


	m_gameEngine = new GameEngine;
	m_gameEngine->Init(m_hWnd);
}

void GameProcess::Loop()
{
	MSG windowsMessage = {};
	while (true)
	{
		// Update
		windowsMessage = m_winInit->ProcessMessage();

		if (windowsMessage.message == WM_QUIT)
		{
			break;
		}

		FixedUpdate();
		Update();

		PreRender();
		Render();
		PostRender();

		LateUpdate();
	}
}

void GameProcess::Release()
{
	delete m_winInit;
	m_winInit = nullptr;
	m_gameEngine->Release();

	delete m_gameEngine;

	m_gameEngine = nullptr;
}

void GameProcess::FixedUpdate()
{
}

void GameProcess::Update()
{
	m_gameEngine->Update();
}

void GameProcess::LateUpdate()
{
}

void GameProcess::PreRender()
{
	/*PatBlt(m_hdc,0,0,1920,1080,BLACKNESS);*/
	GRAPHICS.BeginRender();
	GRAPHICS.ClearScreen(0.0f, 0.0f, 0.0f);
}

void GameProcess::Render()
{
	/*m_gameEngine->Render(m_hdc);*/
	m_gameEngine->Render(GRAPHICS.GetContext());
}

void GameProcess::PostRender()
{
	/*HDC hdc = GetDC(m_hWnd);

	BitBlt(hdc,0,0,1920,1080,m_hdc,0,0,SRCCOPY);
	ReleaseDC(m_hWnd, hdc);*/
	GRAPHICS.EndRender();
}
