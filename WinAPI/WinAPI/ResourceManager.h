#pragma once

#include <windows.h>
#include <unordered_map>
#include <string>
#include <d2d1_1.h>
#include <wincodec.h> 

class ResourceManager
{
public:
    void Init();
    void Release();

    void AddImage(const std::string& key,const std::string& path);
    ID2D1Bitmap* GetImage(const std::string& key);
    //Gdiplus::Bitmap* GetImage(const std::string& key);

private:
    ID2D1Bitmap* LoadBitmapFromFile(const std::wstring& path);
private:
    std::unordered_map<std::string, ID2D1Bitmap*> m_images;
    IWICImagingFactory* m_wicFactory;
};
