#include "TileMap.h"
#include "CameraManager.h"
#include "DrawUtil.h"
#include "ResourceManager.h"
#include "TileManager.h"

void TileMap::Init()
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            m_tiles[y][x].tileX = x;
            m_tiles[y][x].tileY = y;
            m_tiles[y][x].tileType = TileType::FLOOR;
        }
    }
}

void TileMap::Render(ID2D1DeviceContext* context,ResourceManager* resourceManager)
{
    int TILE_W = TileManager::GetInstance().GetTileWidth();
    int TILE_H = TileManager::GetInstance().GetTileHeight();

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen({ (float)x, (float)y });
            float screenX = screen.x - CameraManager::GetInstance().GetX();
            float screenY = screen.y - CameraManager::GetInstance().GetY();

            ID2D1Bitmap* bitmap = nullptr;
            if (m_tiles[y][x].tileType == TileType::FLOOR)
            {
                bitmap = resourceManager->GetImage("Tile_W");
            }

            if (bitmap == nullptr)
                continue;

            // 타일 이미지를 그리드 폭(TILE_W)에 맞춰서, 중심을 기준으로 배치
            D2D1_RECT_F destRect = D2D1::RectF(
                screenX - TILE_W / 2.0f,
                screenY,
                screenX + TILE_W / 2.0f,
                screenY + TILE_H
            );

            D2D1_SIZE_F bmpSize = bitmap->GetSize();
            D2D1_RECT_F srcRect = D2D1::RectF(0, 0, bmpSize.width, bmpSize.height);

            DrawBitmap(context, bitmap, destRect, srcRect, false);
        }
    }
}

