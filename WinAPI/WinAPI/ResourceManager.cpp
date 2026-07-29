#include "ResourceManager.h"
#include "Graphics.h"

#pragma comment(lib, "windowscodecs.lib")

void ResourceManager::Init()
{
    m_wicFactory = nullptr;
    // WIC 팩토리 생성 (COM은 프로그램 어딘가에서 CoInitialize 되어있어야 함)
    CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_wicFactory)
    );
}

void ResourceManager::Release()
{
    for (auto& image : m_images)
    {
        if (image.second)
            image.second->Release();
    }
    m_images.clear();

    if (m_wicFactory)
    {
        m_wicFactory->Release();
        m_wicFactory = nullptr;
    }
}

void ResourceManager::AddImage(const std::string& key, const std::string& path)
{
    std::wstring wpath( path.begin(), path.end());

    ID2D1Bitmap* bitmap = LoadBitmapFromFile(wpath);
    if (bitmap == nullptr)
        return;

    m_images[key] = bitmap;
}

ID2D1Bitmap* ResourceManager::GetImage(const std::string& key)
{
    auto it = m_images.find(key);
    if (it == m_images.end())
        return nullptr;
    return it->second;
}

ID2D1Bitmap* ResourceManager::LoadBitmapFromFile(const std::wstring& path)
{
    if (!m_wicFactory) return nullptr;

    IWICBitmapDecoder* decoder = nullptr;
    HRESULT hr = m_wicFactory->CreateDecoderFromFilename(
        path.c_str(),
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &decoder
    );
    if (FAILED(hr)) return nullptr;

    IWICBitmapFrameDecode* frame = nullptr;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr))
    {
        decoder->Release();
        return nullptr;
    }

    // D2D는 32bpp premultiplied BGRA 포맷을 요구하므로 포맷 변환
    IWICFormatConverter* converter = nullptr;
    hr = m_wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr))
    {
        frame->Release();
        decoder->Release();
        return nullptr;
    }

    hr = converter->Initialize(
        frame,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom
    );
    if (FAILED(hr))
    {
        converter->Release();
        frame->Release();
        decoder->Release();
        return nullptr;
    }

    ID2D1Bitmap* bitmap = nullptr;
    hr = GRAPHICS.GetContext()->CreateBitmapFromWicBitmap(
        converter,
        nullptr,
        &bitmap
    );

    converter->Release();
    frame->Release();
    decoder->Release();

    if (FAILED(hr)) return nullptr;

    return bitmap;
}
