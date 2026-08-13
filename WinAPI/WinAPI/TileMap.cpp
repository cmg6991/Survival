#include "TileMap.h"
#include "CameraManager.h"
#include "DrawUtil.h"
#include "ResourceManager.h"
#include "TileManager.h"
#include "pch.h"

void TileMap::Init()
{
    m_width = 0;
    m_height = 0;

    m_tileImageKeys[TileType::FLOOR] = "Tile_W";
    m_tileImageKeys[TileType::GRASS] = "Tile_W";
    m_tileImageKeys[TileType::ROAD] = "Tile_W";
    m_tileImageKeys[TileType::WATER] = "Tile_W";

    AutotileConfig roadConfig;
    roadConfig.imageKey = "GrassRoad";
    roadConfig.tileW = 64;
    roadConfig.tileH = 32;
    roadConfig.bitN = 2;
    roadConfig.bitE = 4;
    roadConfig.bitS = 8;
    roadConfig.bitW = 1;
    roadConfig.bitmaskMap[0] = { 3, 3 };  // 0000: 고립
    roadConfig.bitmaskMap[1] = { 3, 2 };  // 0001: N만
    roadConfig.bitmaskMap[2] = { 2, 3 };  // 0010: E만
    roadConfig.bitmaskMap[3] = { 2, 2 };  // 0011: NE
    roadConfig.bitmaskMap[4] = { 3, 0 };  // 0100: S만
    roadConfig.bitmaskMap[5] = { 3, 1 };  // 0101: NS직선
    roadConfig.bitmaskMap[6] = { 2, 0 };  // 0110: SE
    roadConfig.bitmaskMap[7] = { 2, 1 };  // 0111: NES(T)
    roadConfig.bitmaskMap[8] = { 0, 3 };  // 1000: W만
    roadConfig.bitmaskMap[9] = { 0, 2 };  // 1001: NW
    roadConfig.bitmaskMap[10] = { 1, 3 };  // 1010: EW직선
    roadConfig.bitmaskMap[11] = { 1, 2 };  // 1011: NEW(T)
    roadConfig.bitmaskMap[12] = { 0, 0 };  // 1100: SW
    roadConfig.bitmaskMap[13] = { 0, 1 };  // 1101: NSW(T)
    roadConfig.bitmaskMap[14] = { 1, 0 };  // 1110: ESW(T)
    roadConfig.bitmaskMap[15] = { 0, 8 };  // 1111: 사거리
    m_autotiles[TileType::ROAD] = roadConfig;

    AutotileConfig waterConfig;
    waterConfig.imageKey = "WaterTile";
    waterConfig.tileW = 64;
    waterConfig.tileH = 32;
    waterConfig.bitN = 1;
    waterConfig.bitE = 2;
    waterConfig.bitS = 4;
    waterConfig.bitW = 8;
    waterConfig.bitmaskMap[0] = { 0, 1 };  // 0000: 고립
    waterConfig.bitmaskMap[1] = { 2, 1 };  // 0001: N만 -> 원본에 없어 사방연결로 대체
    waterConfig.bitmaskMap[2] = { 3, 0 };  // 0010: E만
    waterConfig.bitmaskMap[3] = { 1, 0 };  // 0011: NE
    waterConfig.bitmaskMap[4] = { 2, 1 };  // 0100: S만 -> 원본에 없어 사방연결로 대체
    waterConfig.bitmaskMap[5] = { 2, 1 };  // 0101: NS직선 -> 원본에 없어 사방연결로 대체
    waterConfig.bitmaskMap[6] = { 0, 0 };  // 0110: SE
    waterConfig.bitmaskMap[7] = { 1, 1 };  // 0111: NES(T)
    waterConfig.bitmaskMap[8] = { 2, 1 };  // 1000: W만 -> 원본에 없어 사방연결로 대체
    waterConfig.bitmaskMap[9] = { 0, 2 };  // 1001: NW
    waterConfig.bitmaskMap[10] = { 2, 1 };  // 1010: EW직선 -> 원본에 없어 사방연결로 대체
    waterConfig.bitmaskMap[11] = { 2, 1 };  // 1011: NEW(T) -> 원본에 없어 사방연결로 대체
    waterConfig.bitmaskMap[12] = { 3, 3 };  // 1100: SW
    waterConfig.bitmaskMap[13] = { 1, 2 };  // 1101: NSW(T)
    waterConfig.bitmaskMap[14] = { 2, 0 };  // 1110: ESW(T)
    waterConfig.bitmaskMap[15] = { 2, 1 };  // 1111: 사방연결
    m_autotiles[TileType::WATER] = waterConfig;
}

void TileMap::LoadFromMapData(const vector<string>& mapData)
{
    m_height = (int)mapData.size();
    m_width = 0;

    for (const string& row : mapData)
    {
        m_width = max(m_width, (int)row.size());
    }

    m_tiles.assign(m_height,vector<TileType>(m_width, TileType::FLOOR));

    for (int y = 0; y < m_height; y++)
    {
        const string& row = mapData[y];

        for (int x = 0; x < (int)row.size(); x++)
        {
            if (x >= m_width)
                continue;

            switch (row[x])
            {
            case 'R':
                m_tiles[y][x] = TileType::ROAD;
                break;
            case 'L':
                m_tiles[y][x] = TileType::WATER;
                break;
            case 'g':
                m_tiles[y][x] = TileType::GRASS;
                break;

            default:
                m_tiles[y][x] = TileType::FLOOR;
                break;
            }
        }
    }
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

    //ID2D1Bitmap* bitmap = resourceManager->GetImage("Tile_W");

    //ID2D1Bitmap* floorBitmap = resourceManager->GetImage("Tile_W");
    //ID2D1Bitmap* pathBitmap = resourceManager->GetImage("GrassRoad");

    /*D2D1_SIZE_F bmpSize = floorBitmap->GetSize();
    D2D1_RECT_F srcRect = D2D1::RectF(0, 0, bmpSize.width, bmpSize.height);*/

    unordered_map<TileType, ID2D1Bitmap*> baseCache;
    for (auto& pair : m_tileImageKeys)
        baseCache[pair.first] = resourceManager->GetImage(pair.second);

    unordered_map<TileType, ID2D1Bitmap*> autotileCache;
    for (auto& pair : m_autotiles)
        autotileCache[pair.first] = resourceManager->GetImage(pair.second.imageKey);

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
            //DrawBitmap(context, bitmap, destRect, srcRect, false);
            TileType type = GetTile(x, y);

            //// 3-1. 바닥은 항상 먼저 깔아줌 (ROAD여도 그 밑에 잔디는 깔려있어야 자연스러움)
            //DrawBitmap(context, floorBitmap, destRect, srcRect, false);

            //// 3-2. 도로면 그 위에 오토타일 조각을 덧그림
            //if (type == TileType::ROAD && pathBitmap != nullptr)
            //{
            //    int bitmask = GetPathBitmask(x, y);
            //    D2D1_RECT_F pathSrcRect = GetPathSrcRec(bitmask);
            //    DrawBitmap(context, pathBitmap, destRect, pathSrcRect, false);
            //}
            auto baseIt = baseCache.find(type);
            ID2D1Bitmap* baseBitmap = (baseIt != baseCache.end()) ? baseIt->second : baseCache[TileType::FLOOR];
            if (baseBitmap != nullptr)
            {
                D2D1_SIZE_F size = baseBitmap->GetSize();
                D2D1_RECT_F srcRect = D2D1::RectF(0, 0, size.width, size.height);
                DrawBitmap(context, baseBitmap, destRect, srcRect, false);
            }

            // 2. 이 타입이 오토타일 레이어로 등록돼 있으면 이웃 검사해서 조각 덧그림
            auto autoIt = m_autotiles.find(type);
            if (autoIt != m_autotiles.end())
            {
                ID2D1Bitmap* overlayBitmap = autotileCache[type];
                if (overlayBitmap != nullptr)
                {
                    int bitmask = GetPathBitmask(x, y, type);
                    D2D1_RECT_F overlaySrcRect = GetSrcRect(autoIt->second, bitmask);
                    DrawBitmap(context, overlayBitmap, destRect, overlaySrcRect, false);
                }
            }
        }
    }
}

bool TileMap::IsType(int x, int y, TileType type) const
{
    return GetTile(x, y) == type;
}

TileType TileMap::GetTile(int x, int y) const
{
    if (y < 0 || y >= m_height || x < 0 || x >= m_width)
        return TileType::FLOOR;
    return m_tiles[y][x];
}

void TileMap::SetTile(int x, int y, TileType type)
{
    if (y < 0 || y >= m_height || x < 0 || x >= m_width) return;
    m_tiles[y][x] = type;
}

int TileMap::GetPathBitmask(int x, int y,TileType type) const
{
    auto it = m_autotiles.find(type);
    if (it == m_autotiles.end()) return 0;

    const AutotileConfig& cfg = it->second;

    int mask = 0;
    if (IsType(x, y - 1, type)) mask |= cfg.bitN;
    if (IsType(x + 1, y, type)) mask |= cfg.bitE;
    if (IsType(x, y + 1, type)) mask |= cfg.bitS;
    if (IsType(x - 1, y, type)) mask |= cfg.bitW;
    return mask;
}

D2D1_RECT_F TileMap::GetSrcRect(const AutotileConfig& config, int bitmask) const
{
    const TileCoord& coord = config.bitmaskMap[bitmask];
    float left = (float)(coord.col * config.tileW);
    float top = (float)(coord.row * config.tileH);
    return D2D1::RectF(left, top, left + config.tileW, top + config.tileH);
}

//D2D1_RECT_F TileMap::GetPathSrcRec(int bitmask) const
//{
//    const TileCoord& coord = m_pathTileMap[bitmask];
//    float left = (float)(coord.col * PATH_TILE_W);
//    float top = (float)(coord.row * PATH_TILE_H);
//    return D2D1::RectF(left, top, left + PATH_TILE_W, top + PATH_TILE_H);
//}