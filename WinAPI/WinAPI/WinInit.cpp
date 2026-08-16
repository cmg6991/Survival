#include "WinInit.h"
#include "GameProcess.h"
#include "InputManager.h"
#include <windowsx.h>

WinInit::WinInit() :m_hInst(NULL), m_hWnd(NULL)
{
}

WinInit::~WinInit()
{
}

HWND WinInit::Init(HINSTANCE hInstance)
{
	MyRegisterClass(hInstance);
	InitInstance(hInstance);
	return m_hWnd;
}

MSG WinInit::ProcessMessage()
{
	/*MSG msg = {};

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);
	}

	return msg;*/
	MSG msg = {};

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return msg;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg;
}

void WinInit::Release()
{
}

LRESULT WinInit::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_MOUSEMOVE:
	{
		float x = static_cast<float>(GET_X_LPARAM(lParam));
		float y = static_cast<float>(GET_Y_LPARAM(lParam));

		InputManager::GetInstance().SetMousePosition(x, y);

		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam); // 휠 회전 크기
		//short keyState = GET_KEYSTATE_WPARAM(wParam); // 휠을 돌릴 때의 키 상태 (Ctrl, Shift 등)

		//int x = LOWORD(lParam); // 마우스 X 좌표
		//int y = HIWORD(lParam); // 마우스 Y 좌표

		InputManager::GetInstance().SetWheelPower(zDelta);

		return 0;
	}

	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void WinInit::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = hInstance;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
	wcex.lpszClassName = L"WinAPI";

	RegisterClassEx(&wcex);
}

BOOL WinInit::InitInstance(HINSTANCE hInstance)
{
	m_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	m_hWnd = CreateWindow(
		L"WinAPI",
		L"WinAPIProject",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1280,
		720,
		NULL,
		NULL,
		hInstance,
		nullptr);

	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	return TRUE;
}
