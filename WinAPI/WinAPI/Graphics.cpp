#include "Graphics.h"

Graphics::Graphics()
{
	m_hwnd = nullptr;
	m_d3dDevice = nullptr;
	m_d3dContext = nullptr;
	m_swapChain = nullptr;
	m_factory = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_targetBitmap = nullptr;
	m_brush = nullptr;
	m_textFormat = nullptr;
	m_writeFactory = nullptr;
}

Graphics::~Graphics()
{
	Release();
}

bool Graphics::Init(HWND hWnd)
{
#define CHECK_HR(hr, msg)\
if(FAILED(hr))\
{\
    MessageBox(nullptr, msg, L"Graphics Error", MB_OK);\
    return false;\
}

	m_hwnd = hWnd;

	UINT flag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	HRESULT hr;

	D3D_FEATURE_LEVEL level;

	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flag,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&m_d3dDevice,
		&level,
		&m_d3dContext
	);
	CHECK_HR(hr, L"D3D11CreateDevice 실패");
	if (FAILED(hr)) return false;

	IDXGIDevice* dxgiDevice = nullptr;
	hr = m_d3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
	CHECK_HR(hr, L"QueryInterface DXGI 실패");
	if (FAILED(hr)) return false;

	IDXGIAdapter* adapter = nullptr;
	hr = dxgiDevice->GetAdapter(&adapter);
	CHECK_HR(hr, L"GetAdapter 실패");
	if (FAILED(hr))
	{
		dxgiDevice->Release();
		return false;
	}

	IDXGIFactory2* factory = nullptr;
	hr = adapter->GetParent(IID_PPV_ARGS(&factory));
	CHECK_HR(hr, L"DXGI Factory 실패");
	if (FAILED(hr))
	{
		adapter->Release();
		dxgiDevice->Release();
		return false;
	}

	// 3. 스왑체인 생성
	RECT rc;
	GetClientRect(hWnd, &rc);
	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.Width = rc.right - rc.left;
	desc.Height = rc.bottom - rc.top;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.BufferCount = 2;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count = 1;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

	hr = factory->CreateSwapChainForHwnd(
		m_d3dDevice,
		hWnd,
		&desc,
		nullptr,
		nullptr,
		&m_swapChain
	);
	CHECK_HR(hr, L"SwapChain 생성 실패");
	if (FAILED(hr))
	{
		factory->Release();
		adapter->Release();
		dxgiDevice->Release();
		return false;
	}

	// 4. D2D 팩토리 / 디바이스 생성 (dxgiDevice가 아직 살아있는 상태에서 사용)
	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory1),
		nullptr,
		reinterpret_cast<void**>(&m_factory)
	);

	CHECK_HR(hr, L"D2D Factory 실패");
	if (FAILED(hr))
	{
		factory->Release();
		adapter->Release();
		dxgiDevice->Release();
		return false;
	}

	hr = m_factory->CreateDevice(dxgiDevice, &m_device);
	CHECK_HR(hr, L"D2D Device 실패");
	if (FAILED(hr))
	{
		factory->Release();
		adapter->Release();
		dxgiDevice->Release();
		return false;
	}

	// 이제 dxgiDevice, adapter, factory 다 썼으니 해제
	factory->Release();
	adapter->Release();
	dxgiDevice->Release();

	// 5. D2D 디바이스 컨텍스트 생성
	hr = m_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_deviceContext);
	CHECK_HR(hr, L"D2D DeviceContext 실패");
	if (FAILED(hr)) return false;

	// 6. 스왑체인의 백버퍼로부터 타겟 비트맵 생성
	IDXGISurface* surface = nullptr;
	hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
	CHECK_HR(hr, L"SwapChain GetBuffer 실패");
	if (FAILED(hr)) return false;

	D2D1_BITMAP_PROPERTIES1 prop =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET |
			D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED
			)
		);

	hr = m_deviceContext->CreateBitmapFromDxgiSurface(
		surface,
		&prop,
		&m_targetBitmap
	);
	CHECK_HR(hr, L"Target Bitmap 생성 실패");
	surface->Release();
	if (FAILED(hr)) return false;

	m_deviceContext->SetTarget(m_targetBitmap);

	// 7. 브러시 생성
	hr = m_deviceContext->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White),
		&m_brush
	);
	CHECK_HR(hr, L"Brush 생성 실패");
	if (FAILED(hr)) return false;

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_writeFactory)
	);

	CHECK_HR(hr, L"DirectWrite Factory 생성 실패");
	if (FAILED(hr)) return false;

	hr = m_writeFactory->CreateTextFormat(
		L"온글잎 도람체",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"",
		&m_textFormat
	);

	CHECK_HR(hr, L"TextFormat 생성 실패");
	if (FAILED(hr)) return false;

	return true;
}

void Graphics::BeginRender()
{
	m_deviceContext->BeginDraw();
}

void Graphics::EndRender()
{
	m_deviceContext->EndDraw();
	m_swapChain->Present(1, 0);
}

void Graphics::ClearScreen(float r, float g, float b)
{
	m_deviceContext->Clear(D2D1::ColorF(r, g, b));
}

//void Graphics::DrawCircle(float x, float y, float radius, float r, float g, float b, float a)
//{
//	brush->SetColor(D2D1::ColorF(r, g, b, a));
//	deviceContext->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), brush.Get(), 3.0f);
//}

void Graphics::DrawString(const wchar_t* text, float x, float y)
{
	m_deviceContext->DrawTextW(
		text,
		wcslen(text),
		m_textFormat,
		D2D1::RectF(
			x,
			y,
			x+500,
			y+50
		),
		m_brush
	);
}

void Graphics::DrawString(const wchar_t* text, float x, float y, float fontSize)
{
	IDWriteTextFormat* format = nullptr;

	m_writeFactory->CreateTextFormat(
		L"온글잎 도람체",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"ko-KR",
		&format);

	m_deviceContext->DrawTextW(
		text,
		(UINT32)wcslen(text),
		format,
		D2D1::RectF(x, y, x + 500, y + 100),
		m_brush);
}

void Graphics::DrawRect(float x, float y, float width, float height, D2D1::ColorF color, float thickness)
{
	ID2D1SolidColorBrush* brush = nullptr;
	m_deviceContext->CreateSolidColorBrush(color, &brush);
	if (brush == nullptr) return;

	D2D1_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);
	m_deviceContext->DrawRectangle(rect, brush, thickness);

	//brush->Release();
}

void Graphics::FillRect(float x, float y, float width, float height, D2D1::ColorF color)
{
	ID2D1SolidColorBrush* brush = nullptr;
	m_deviceContext->CreateSolidColorBrush(color, &brush);
	if (brush == nullptr) return;

	D2D1_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);
	m_deviceContext->FillRectangle(rect, brush);

	//brush->Release();
}

void Graphics::DrawBitmapUI(ID2D1Bitmap* bitmap, float x, float y, float width, float height)
{
	if (bitmap == nullptr) return;

	D2D1_SIZE_F size = bitmap->GetSize();
	D2D1_RECT_F destRect = D2D1::RectF(x, y, x + width, y + height);
	D2D1_RECT_F srcRect = D2D1::RectF(0, 0, size.width, size.height);

	m_deviceContext->DrawBitmap(
		bitmap,
		destRect,
		1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		srcRect
	);
}


void Graphics::Release()
{
	if (m_textFormat)
	{
		m_textFormat->Release();
		m_textFormat = nullptr;
	}


	if (m_writeFactory)
	{
		m_writeFactory->Release();
		m_writeFactory = nullptr;
	}
	if (m_brush) { m_brush->Release(); m_brush = nullptr; }
	if (m_targetBitmap) { m_targetBitmap->Release(); m_targetBitmap = nullptr; }
	if (m_deviceContext) { m_deviceContext->Release(); m_deviceContext = nullptr; }
	if (m_device) { m_device->Release(); m_device = nullptr; }
	if (m_factory) { m_factory->Release(); m_factory = nullptr; }
	if (m_swapChain) { m_swapChain->Release(); m_swapChain = nullptr; }
	if (m_d3dContext) { m_d3dContext->Release(); m_d3dContext = nullptr; }
	if (m_d3dDevice) { m_d3dDevice->Release(); m_d3dDevice = nullptr; }
}