#include "TileMap.h"
#include "CameraManager.h"
#include "DrawUtil.h"
#include "ResourceManager.h"
#include "TileManager.h"
#include "pch.h"

void TileMap::Init()
{
    
}

void TileMap::Render(ID2D1DeviceContext* context,ResourceManager* resourceManager)
{
    int TILE_W = TileManager::GetInstance().GetTileWidth();
    int TILE_H = TileManager::GetInstance().GetTileHeight();

    //for (int y = 0; y < HEIGHT; y++)
    //{
    //    for (int x = 0; x < WIDTH; x++)
    //    {
    //        MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen({ (float)x, (float)y });
    //        float screenX = screen.x - CameraManager::GetInstance().GetX();
    //        float screenY = screen.y - CameraManager::GetInstance().GetY();

    //        ID2D1Bitmap* bitmap = nullptr;
    //        if (m_tiles[y][x].tileType == TileType::FLOOR)
    //        {
    //            bitmap = resourceManager->GetImage("Tile_W");
    //        }

    //        if (bitmap == nullptr)
    //            continue;

    //        // 타일 이미지를 그리드 폭(TILE_W)에 맞춰서, 중심을 기준으로 배치
    //        D2D1_RECT_F destRect = D2D1::RectF(
    //            screenX - TILE_W / 2.0f,
    //            screenY,
    //            screenX + TILE_W / 2.0f,
    //            screenY + TILE_H
    //        );

    //        D2D1_SIZE_F bmpSize = bitmap->GetSize();
    //        D2D1_RECT_F srcRect = D2D1::RectF(0, 0, bmpSize.width, bmpSize.height);

    //        DrawBitmap(context, bitmap, destRect, srcRect, false);
    //    }
    //}
	//float camX = CameraManager::GetInstance().GetX();
	//float camY = CameraManager::GetInstance().GetY();

	//const int SCREEN_W = 1280;
	//const int SCREEN_H = 780;

	//// 화면 네 귀퉁이의 월드 좌표를 타일 좌표로 역변환해서
	//// 지금 화면에 보이는 타일 범위(minX~maxX, minY~maxY)를 계산
	//MathEngine::Vector2 corners[4] =
	//{
	//	{ camX,             camY },
	//	{ camX + SCREEN_W,  camY },
	//	{ camX,             camY + SCREEN_H },
	//	{ camX + SCREEN_W,  camY + SCREEN_H }
	//};

	//int minX = INT_MAX, maxX = INT_MIN;
	//int minY = INT_MAX, maxY = INT_MIN;

	//for (int i = 0; i < 4; i++)
	//{
	//	MathEngine::Vector2 tile = TileManager::GetInstance().ScreenToTile(corners[i]);
	//	int tx = (int)std::floor(tile.x);
	//	int ty = (int)std::floor(tile.y);

	//	minX = min(minX, tx);
	//	maxX = max(maxX, tx);
	//	minY = min(minY, ty);
	//	maxY = max(maxY, ty);
	//}

	//// 화면 경계에서 타일이 잘려 보이지 않도록 여유(margin) 추가
	//const int margin = 2;
	//minX -= margin; maxX += margin;
	//minY -= margin; maxY += margin;

	//ID2D1Bitmap* bitmap = resourceManager->GetImage("Tile_W");
	//if (bitmap == nullptr) return;

	//D2D1_SIZE_F bmpSize = bitmap->GetSize();
	//D2D1_RECT_F srcRect = D2D1::RectF(0, 0, bmpSize.width, bmpSize.height);

	//for (int y = minY; y <= maxY; y++)
	//{
	//	for (int x = minX; x <= maxX; x++)
	//	{
	//		MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen({ (float)x, (float)y });
	//		float screenX = screen.x - camX;
	//		float screenY = screen.y - camY;

	//		D2D1_RECT_F destRect = D2D1::RectF(
	//			screenX - TILE_W / 2.0f,
	//			screenY,
	//			screenX + TILE_W / 2.0f,
	//			screenY + TILE_H
	//		);

	//		DrawBitmap(context, bitmap, destRect, srcRect, false);
	//	}
	//}

    float camX = CameraManager::GetInstance().GetX();
    float camY = CameraManager::GetInstance().GetY();
    const int SCREEN_W = 1280;
    const int SCREEN_H = 720;

    MathEngine::Vector2 corners[4] =
    {
        { camX,            camY },
        { camX + SCREEN_W, camY },
        { camX,            camY + SCREEN_H },
        { camX + SCREEN_W, camY + SCREEN_H }
    };

    int minX = INT_MAX, maxX = INT_MIN;
    int minY = INT_MAX, maxY = INT_MIN;
    for (int i = 0; i < 4; i++)
    {
        MathEngine::Vector2 tile = TileManager::GetInstance().ScreenToTile(corners[i]);
        int tx = (int)std::floor(tile.x);
        int ty = (int)std::floor(tile.y);
        minX = min(minX, tx);
        maxX = max(maxX, tx);
        minY = min(minY, ty);
        maxY = max(maxY, ty);
    }

    const int margin = 2;
    minX -= margin; maxX += margin;
    minY -= margin; maxY += margin;

    // ★ 맵 크기로 clamp 하지 않음 -> 카메라가 어디로 가든 바닥이 계속 깔림

    ID2D1Bitmap* bitmap = resourceManager->GetImage("Tile_W");
    if (bitmap == nullptr) return;

    D2D1_SIZE_F bmpSize = bitmap->GetSize();
    D2D1_RECT_F srcRect = D2D1::RectF(0, 0, bmpSize.width, bmpSize.height);

    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen({ (float)x, (float)y });
            float screenX = screen.x - camX;
            float screenY = screen.y - camY;

            D2D1_RECT_F destRect = D2D1::RectF(
                screenX - TILE_W / 2.0f,
                screenY,
                screenX + TILE_W / 2.0f,
                screenY + TILE_H
            );
            DrawBitmap(context, bitmap, destRect, srcRect, false);
        }
    }
}

