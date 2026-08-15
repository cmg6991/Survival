#pragma once

#include <Windows.h>

#include <d3d11.h>
#include <dxgi1_2.h>

#include <d2d1_1.h>
#include <dwrite.h>

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dwrite.lib")

#define GRAPHICS Graphics::GetInstance()

class Graphics
{
public:
	static Graphics& GetInstance()
	{
		static Graphics instance;
		return instance;
	}

	bool Init(HWND hWnd);

	void BeginRender();
	void EndRender();

	void ClearScreen(float r, float g, float b);
	ID2D1DeviceContext* GetContext()
	{
		return m_deviceContext;
	}

	void DrawString(const wchar_t* text, float x, float y);
	void DrawString(const wchar_t* text, float x, float y, D2D1::ColorF color, float fontSize);

	void DrawRect(float x, float y, float width, float height, D2D1::ColorF color, float thickness = 2.0f);
	void FillRect(float x, float y, float width, float height, D2D1::ColorF color);
	void DrawBitmapUI(ID2D1Bitmap* bitmap, float x, float y, float width, float height);
	void DrawCircle(float centerX, float centerY, float radius, D2D1::ColorF color, float thickness = 2.0f);
	void FillRoundedRect(float x,float y,float width,float height,float radius,D2D1::ColorF color);
	void DrawRoundedRect(float x,float y,float width,float height,float radius,D2D1::ColorF color,float thickness = 1.0f);

	float MeasureTextWidth(const wchar_t* text, float fontSize);

private:
	Graphics();
	~Graphics();


	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	HWND m_hwnd;


	// D3D
	ID3D11Device* m_d3dDevice;
	ID3D11DeviceContext* m_d3dContext;


	// DXGI
	IDXGISwapChain1* m_swapChain;


	// D2D
	ID2D1Factory1* m_factory;
	ID2D1Device* m_device;

	ID2D1DeviceContext* m_deviceContext;

	ID2D1Bitmap1* m_targetBitmap;


	ID2D1SolidColorBrush* m_brush;

private:
	IDWriteFactory* m_writeFactory;
	IDWriteTextFormat* m_textFormat;

private:

	void Release();
};