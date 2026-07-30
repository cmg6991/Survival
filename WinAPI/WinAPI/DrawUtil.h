#pragma once
#include <d2d1_1.h>

inline void DrawBitmap(
    ID2D1DeviceContext* context,
    ID2D1Bitmap* bitmap,
    const D2D1_RECT_F& destRect,
    const D2D1_RECT_F& srcRect,
    bool flip = false)
{
    if (bitmap == nullptr)
        return;

    if (!flip)
    {
        context->DrawBitmap(
            bitmap,
            destRect,
            1.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
            srcRect
        );
    }
    else
    {
        D2D1_MATRIX_3X2_F oldTransform;
        context->GetTransform(&oldTransform);

        float centerX = (destRect.left + destRect.right) * 0.5f;
        float centerY = (destRect.top + destRect.bottom) * 0.5f;

        D2D1_MATRIX_3X2_F flipMatrix =
            D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, D2D1::Point2F(centerX, centerY)) * oldTransform;

        context->SetTransform(flipMatrix);
        context->DrawBitmap(bitmap, destRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, srcRect);
        context->SetTransform(oldTransform);
    }
}

// 이미지 전체를 그대로, 지정한 x,y 위치(좌상단 기준)에 그리는 편의 함수
inline void DrawBitmap(
    ID2D1DeviceContext* context,
    ID2D1Bitmap* bitmap,
    float x,
    float y)
{
    if (bitmap == nullptr)
        return;

    D2D1_SIZE_F size = bitmap->GetSize();

    D2D1_RECT_F destRect = D2D1::RectF(x, y, x + size.width, y + size.height);
    D2D1_RECT_F srcRect = D2D1::RectF(0, 0, size.width, size.height);

    context->DrawBitmap(
        bitmap,
        destRect,
        1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
        srcRect
    );
}