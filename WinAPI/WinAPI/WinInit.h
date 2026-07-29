#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class WinInit
{
public:
	WinInit();
	~WinInit();

	/// ---------- Event ----------
	HWND Init(HINSTANCE hInstance);
	MSG ProcessMessage();
	void Release();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	/// ---------- Init Window ----------
	void MyRegisterClass(HINSTANCE hInstance);
	BOOL InitInstance(HINSTANCE hInstance);

private:
	/// ---------- Member ----------
	HINSTANCE	m_hInst;
	HWND		m_hWnd;
};

