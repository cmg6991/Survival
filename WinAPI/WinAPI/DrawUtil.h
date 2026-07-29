#pragma once

#include <windows.h>

//inline void DrawBitmap(
//    HDC hdc,
//    HBITMAP bitmap,
//    int x,
//    int y)
//{
//    HDC memDC =
//        CreateCompatibleDC(hdc);
//
//    HBITMAP oldBitmap =
//        (HBITMAP)SelectObject(
//            memDC,
//            bitmap);
//
//    BITMAP info;
//
//    GetObject(
//        bitmap,
//        sizeof(BITMAP),
//        &info);
//
//    BitBlt(
//        hdc,
//        x,
//        y,
//        info.bmWidth,
//        info.bmHeight,
//        memDC,
//        0,
//        0,
//        SRCCOPY);
//
//    SelectObject(
//        memDC,
//        oldBitmap);
//
//    DeleteDC(memDC);
//}
#include <gdiplus.h>

using namespace Gdiplus;

inline void DrawBitmap(Graphics& graphics,
    Bitmap* bitmap,
    int x,
    int y)
{
    if (bitmap == nullptr)
        return;

    graphics.DrawImage(
        bitmap,
        x,
        y,
        bitmap->GetWidth(),
        bitmap->GetHeight());
}