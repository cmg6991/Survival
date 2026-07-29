#pragma once

#include <windows.h>
#include <unordered_map>
#include <string>
#include <gdiplus.h>

class ResourceManager
{
public:
    void Init();
    void Release();

    void AddImage(const std::string& key,const std::string& path);

    Gdiplus::Bitmap* GetImage(const std::string& key);

private:
    std::unordered_map< std::string, Gdiplus::Bitmap*> m_images;
};
