#include "CollisionManager.h"
#include "CameraManager.h"
#include "Graphics.h"
#include "TileManager.h"

CollisionManager::CollisionManager()
{
    for (int y = 0; y < 100; y++)
    {
        for (int x = 0; x < 100; x++)
        {
            m_blockMap[y][x] = false;
        }
    }
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::Init()
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            m_blockMap[y][x] = false;
        }
    }
}

bool CollisionManager::IsBlocked(int x, int y)
{
    if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
    {
        return false;
    }

    return m_blockMap[y][x];
}

void CollisionManager::SetBlocked(int x, int y, bool blocked)
{
    if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
    {
        return;
    }

    m_blockMap[y][x] = blocked;
}

void CollisionManager::RenderDebug(ID2D1DeviceContext* context)
{
    //ID2D1SolidColorBrush* pen = nullptr;
    //context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pen);
    //if (pen == nullptr) return;

    //const int TILE_W = TileManager::GetInstance().GetTileWidth();
    //const int TILE_H = TileManager::GetInstance().GetTileHeight();

    //for (int y = 0; y < HEIGHT; y++)
    //{
    //    for (int x = 0; x < WIDTH; x++)
    //    {
    //        if (!m_blockMap[y][x])
    //            continue;

    //        // SpriteRenderer와 완전히 동일한 변환 함수 사용
    //        MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen({ (float)x, (float)y });

    //        float screenX = screen.x - CameraManager::GetInstance().GetX();
    //        float screenY = screen.y - CameraManager::GetInstance().GetY();
    //        screenY += TILE_H / 2.0f; // SpriteRenderer의 오프셋과 동일하게 맞춤

    //        D2D1_POINT_2F pts[4] =
    //        {
    //            D2D1::Point2F(screenX,                 screenY - TILE_H / 2.0f),
    //            D2D1::Point2F(screenX + TILE_W / 2.0f, screenY),
    //            D2D1::Point2F(screenX,                 screenY + TILE_H / 2.0f),
    //            D2D1::Point2F(screenX - TILE_W / 2.0f, screenY)
    //        };

    //        context->DrawLine(pts[0], pts[1], pen, 2.0f);
    //        context->DrawLine(pts[1], pts[2], pen, 2.0f);
    //        context->DrawLine(pts[2], pts[3], pen, 2.0f);
    //        context->DrawLine(pts[3], pts[0], pen, 2.0f);
    //    }
    //}

    //pen->Release();
}
