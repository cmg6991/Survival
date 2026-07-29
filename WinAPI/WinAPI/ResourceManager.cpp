#include "ResourceManager.h"
#pragma comment(lib, "gdiplus.lib")
#include <gdiplus.h>

using namespace Gdiplus;

void ResourceManager::Init()
{

}

void ResourceManager::Release()
{
    for (auto& image : m_images)
    {
        DeleteObject(image.second);
    }

    m_images.clear();
}

//void ResourceManager::AddImage(
//    const std::string& key,
//    const std::string& path)
//{
//    HBITMAP bitmap =
//        (HBITMAP)LoadImageA(
//            NULL,
//            path.c_str(),
//            IMAGE_BITMAP,
//            0,
//            0,
//            LR_LOADFROMFILE);
//
//    m_images.insert({ key, bitmap });
//}

void ResourceManager::AddImage(
    const std::string& key,
    const std::string& path)
{
    std::wstring wpath(
        path.begin(),
        path.end());

    Bitmap* bitmap =
        Bitmap::FromFile(
            wpath.c_str());

    if (bitmap == nullptr)
        return;

    m_images[key] = bitmap;
}

Gdiplus::Bitmap* ResourceManager::GetImage(
    const std::string& key)
{
    return m_images[key];
}