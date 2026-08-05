#include "GameProcess.h"
#include "WinInit.h"
#include "CameraManager.h"
#include "GameEngine.h"
#include "Graphics.h"
#include <chrono>

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
	AddFontResourceEx(
		L"Resource\\Font\\온글잎 도람체.ttf",
		FR_PRIVATE,
		nullptr);

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

	using namespace std::chrono;

	auto prevTime = steady_clock::now();
	auto fpsTime = steady_clock::now();
	auto frameStart = steady_clock::now();

	int frameCount = 0;
	float fps = 0.0f;
	float frameTime = 0.0f;

	while (true)
	{
		auto currentTime = steady_clock::now();

		m_deltaTime = duration<float>(currentTime - prevTime).count();
		prevTime = currentTime;
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

		frameCount++;

		float elapsed = duration<float>(currentTime - fpsTime).count();

		if (elapsed >= 1.0f)
		{
			fps = frameCount / elapsed;

			frameCount = 0;
			fpsTime = currentTime;
		}

		// Frame Time 계산
		auto frameEnd = steady_clock::now();

		frameTime = duration<float, milli>(frameEnd - frameStart).count();

		frameStart = frameEnd;


		// 표시
		wchar_t title[128];

		swprintf_s(
			title,
			L"FPS : %.1f | Frame : %.3f ms | Delta:%.5f",
			fps,
			frameTime,
			m_deltaTime
		);

		SetWindowText(m_hWnd, title);

		LateUpdate();
	}
}

void GameProcess::Release()
{
	RemoveFontResourceEx(
		L"Resource\\Font\\온글잎 도람체.ttf",
		FR_PRIVATE,
		nullptr);

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
	m_gameEngine->Update(m_deltaTime);
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
