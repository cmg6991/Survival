#include "TileMap.h"
#include "CameraManager.h"
#include "DrawUtil.h"
#include "ResourceManager.h"
#include "TileManager.h"
#include "pch.h"
#include "ObjectSpawner.h"

long long MakeChunkKey(int chunkX,int chunkY)
{
    return(static_cast<long long>(chunkX) << 32) ^static_cast<unsigned int>(chunkY);
}
long long MakeTileKey(int x,int y)
{
    return(static_cast<long long>(x) << 32) ^static_cast<unsigned int>(y);
}

void TileMap::Init()
{
    m_width = 0;
    m_height = 0;

    m_tiles.clear();
    m_proceduralTiles.clear();
    m_generatedChunks.clear();

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
    waterConfig.useDiagonalNeighbors = true;
    waterConfig.bitmaskMap[0] = { 3, 0 };  // 0000: 고립 (원본에 없어 N만 조각으로 대체)
    waterConfig.bitmaskMap[1] = { 2, 0 };  // 0001: N만
    waterConfig.bitmaskMap[2] = { 3, 1 };  // 0010: E만
    waterConfig.bitmaskMap[3] = { 0, 1 };  // 0011: NE
    waterConfig.bitmaskMap[4] = { 0, 0 };  // 0100: S만
    waterConfig.bitmaskMap[5] = { 3, 2 };  // 0101: NS직선
    waterConfig.bitmaskMap[6] = { 0, 3 };  // 0110: SE
    waterConfig.bitmaskMap[7] = { 1, 1 };  // 0111: NES(T)
    waterConfig.bitmaskMap[8] = { 3, 3 };  // 1000: W만
    waterConfig.bitmaskMap[9] = { 2, 1 };  // 1001: NW
    waterConfig.bitmaskMap[10] = { 1, 0 };  // 1010: EW직선
    waterConfig.bitmaskMap[11] = { 2, 2 };  // 1011: NEW(T)
    waterConfig.bitmaskMap[12] = { 2, 3 };  // 1100: SW
    waterConfig.bitmaskMap[13] = { 1, 3 };  // 1101: NSW(T)
    waterConfig.bitmaskMap[14] = { 0, 2 };  // 1110: ESW(T)
    waterConfig.bitmaskMap[15] = { 1, 2 };  // 1111: 사방연결
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
    //int maxChunkX = (m_width - 1) / CHUNK_SIZE;
    //int maxChunkY = (m_height - 1) / CHUNK_SIZE;

    //for (int cy = 0; cy <= maxChunkY; cy++)
    //{
    //    for (int cx = 0; cx <= maxChunkX; cx++)
    //    {
    //        int startX = cx * CHUNK_SIZE;
    //        int startY = cy * CHUNK_SIZE;

    //        // 맵 데이터 영역은 이미 타일이 존재하므로
    //        // 여기서는 오브젝트만 생성
    //        if (m_objectSpawner != nullptr)
    //        {
    //            m_objectSpawner->SpawnChunk(
    //                startX,
    //                startY,
    //                CHUNK_SIZE,
    //                CHUNK_SIZE
    //            );
    //        }

    //        m_generatedChunks.insert(
    //            MakeChunkKey(cx, cy)
    //        );
    //    }
    //}
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
            // -----------------------------------------
            // 오토타일 설정 확인
            // -----------------------------------------
            auto autoIt = m_autotiles.find(type);

            int bitmask = 0;

            if (autoIt != m_autotiles.end())
            {
                bitmask = GetPathBitmask(x, y, type);
            }


            // -----------------------------------------
            // 기본 바닥
            //
            // 일반 타일:
            //     항상 그림
            //
            // WATER:
            //     mask == 15  → 물 중앙 → 안 그림
            //     mask != 15  → 물 경계 → FLOOR 그림
            // -----------------------------------------
            bool drawBase = true;

            if (type == TileType::WATER)
            {
                drawBase = (bitmask != 15);
            }

            if (drawBase)
            {
                ID2D1Bitmap* baseBitmap =
                    baseCache[TileType::FLOOR];

                if (baseBitmap != nullptr)
                {
                    D2D1_SIZE_F size = baseBitmap->GetSize();

                    D2D1_RECT_F srcRect =
                        D2D1::RectF(
                            0,
                            0,
                            size.width,
                            size.height
                        );

                    DrawBitmap(
                        context,
                        baseBitmap,
                        destRect,
                        srcRect,
                        false
                    );
                }
            }


            // -----------------------------------------
            // 오토타일
            // -----------------------------------------
            /*if (autoIt != m_autotiles.end())
            {
                ID2D1Bitmap* overlayBitmap =
                    autotileCache[type];

                if (overlayBitmap != nullptr)
                {
                    D2D1_RECT_F overlaySrcRect =
                        GetSrcRect(
                            autoIt->second,
                            bitmask
                        );

                    DrawBitmap(
                        context,
                        overlayBitmap,
                        destRect,
                        overlaySrcRect,
                        false
                    );
                }
            }*/
            if (autoIt != m_autotiles.end())
            {
                ID2D1Bitmap* overlayBitmap =
                    autotileCache[type];

                if (overlayBitmap != nullptr)
                {
                    D2D1_RECT_F overlaySrcRect =
                        GetSrcRect(
                            autoIt->second,
                            bitmask
                        );

                    // 오토타일끼리 생기는 검은 틈 방지
                    D2D1_RECT_F overlayDestRect = destRect;

                    if (type == TileType::WATER)
                    {
                        const float overlap = 4.0f;

                        overlayDestRect.left -= overlap;
                        overlayDestRect.top -= overlap;
                        overlayDestRect.right += overlap;
                        overlayDestRect.bottom += overlap;
                    }

                    DrawBitmap(
                        context,
                        overlayBitmap,
                        overlayDestRect,
                        overlaySrcRect,
                        false
                    );
                }
            }
        }
    }
}

bool TileMap::IsRoad(int x, int y) const
{
    // 맵 데이터에 있는 실제 ROAD
    if (x >= 0 && x < m_width &&
        y >= 0 && y < m_height)
    {
        if (m_tiles[y][x] == TileType::ROAD)
            return true;
    }

    // 절차적으로 생성되는 ROAD
    if (IsProceduralRoad(x, y))
        return true;

    return false;
}

bool TileMap::IsType(int x, int y, TileType type) const
{
    return GetTile(x, y) == type;
}

TileType TileMap::GetTile(int x, int y) const
{
    if (x >= 0 && x < m_width &&
        y >= 0 && y < m_height)
    {
        TileType mapTile = m_tiles[y][x];

        // 맵에 직접 지정된 ROAD
        if (mapTile == TileType::ROAD)
            return TileType::ROAD;

        // 맵에 직접 지정된 WATER
        if (mapTile == TileType::WATER)
            return TileType::WATER;

        // 기존 맵 영역에서도 절차적 도로를 적용
        if (IsProceduralRoad(x, y))
            return TileType::ROAD;

        return mapTile;
    }
    int chunkX = WorldToChunk(x);
    int chunkY = WorldToChunk(y);

    if (!IsChunkGenerated(chunkX, chunkY))
    {
        const_cast<TileMap*>(this)->EnsureChunk(chunkX, chunkY);
    }

    auto it = m_proceduralTiles.find(MakeTileKey(x, y));

    if (it != m_proceduralTiles.end())
    {
        return it->second;
    }

    if (IsProceduralRoad(x, y))
        return TileType::ROAD;

    return TileType::FLOOR;

}

void TileMap::SetTile(int x, int y, TileType type)
{
    if (x >= 0 &&x < m_width &&y >= 0 &&y < m_height)
    {
        m_tiles[y][x] = type;
        return;
    }
    // 자동 Chunk 영역
    int chunkX =WorldToChunk(x);
    int chunkY =WorldToChunk(y);
    EnsureChunk(chunkX,chunkY);
    m_proceduralTiles[MakeTileKey(x, y)] = type;
}

int TileMap::GetPathBitmask(int x, int y,TileType type) const
{
    /*auto it = m_autotiles.find(type);
    if (it == m_autotiles.end()) return 0;

    const AutotileConfig& cfg = it->second;

    int mask = 0;
    if (IsType(x, y - 1, type)) mask |= cfg.bitN;
    if (IsType(x + 1, y, type)) mask |= cfg.bitE;
    if (IsType(x, y + 1, type)) mask |= cfg.bitS;
    if (IsType(x - 1, y, type)) mask |= cfg.bitW;
    return mask;*/
    auto it = m_autotiles.find(type);
    if (it == m_autotiles.end()) return 0;

    const AutotileConfig& cfg = it->second;
    int mask = 0;

    if (cfg.useDiagonalNeighbors)
    {
        // 대각선 이웃 검사 (물처럼 꼭짓점 노치로 연결을 표현하는 타일용)
        if (IsType(x - 1, y - 1, type)) mask |= cfg.bitN;   // 북서 -> 위쪽 꼭짓점
        if (IsType(x + 1, y - 1, type)) mask |= cfg.bitE;   // 북동 -> 오른쪽 꼭짓점
        if (IsType(x + 1, y + 1, type)) mask |= cfg.bitS;   // 남동 -> 아래쪽 꼭짓점
        if (IsType(x - 1, y + 1, type)) mask |= cfg.bitW;   // 남서 -> 왼쪽 꼭짓점
    }
    else
    {
        // 기존 상하좌우 이웃 검사 (도로처럼 변으로 연결을 표현하는 타일용)
        if (IsType(x, y - 1, type)) mask |= cfg.bitN;
        if (IsType(x + 1, y, type)) mask |= cfg.bitE;
        if (IsType(x, y + 1, type)) mask |= cfg.bitS;
        if (IsType(x - 1, y, type)) mask |= cfg.bitW;
    }
    return mask;
}

D2D1_RECT_F TileMap::GetSrcRect(const AutotileConfig& config, int bitmask) const
{
    const TileCoord& coord = config.bitmaskMap[bitmask];
    float left = (float)(coord.col * config.tileW);
    float top = (float)(coord.row * config.tileH);
    return D2D1::RectF(left, top, left + config.tileW, top + config.tileH);
}

void TileMap::EnsureChunk(int chunkX, int chunkY)
{
    if (IsChunkGenerated(chunkX, chunkY))
        return;

    m_generatedChunks.insert(MakeChunkKey(chunkX, chunkY));
    GenerateChunk(chunkX, chunkY);
}

void TileMap::GenerateChunk(int chunkX, int chunkY)
{
    int startX = chunkX * CHUNK_SIZE;
    int startY = chunkY * CHUNK_SIZE;
    for (int localY = 0;localY < CHUNK_SIZE;localY++)
    {
        for (int localX = 0;localX < CHUNK_SIZE;localX++)
        {
            int worldX =chunkX * CHUNK_SIZE+ localX;
            int worldY =chunkY * CHUNK_SIZE+ localY;

            if (worldX >= 0 && worldX < m_width &&
                worldY >= 0 && worldY < m_height)
            {
                continue;
            }

            TileType type =GenerateProceduralTile(worldX,worldY,chunkX,chunkY);

            m_proceduralTiles[MakeTileKey(worldX,worldY)] = type;
        }
    }
    if (m_objectSpawner != nullptr)
    {
        m_objectSpawner->SpawnChunk(chunkX, chunkY, CHUNK_SIZE, CHUNK_SIZE);
    }
}

bool TileMap::IsChunkGenerated(int chunkX, int chunkY) const
{
    return m_generatedChunks.find(MakeChunkKey(chunkX, chunkY)) != m_generatedChunks.end();
}

int TileMap::WorldToChunk(int worldCoord) const
{
    if (worldCoord >= 0)
        return worldCoord / CHUNK_SIZE;

    return(worldCoord - CHUNK_SIZE + 1)/ CHUNK_SIZE;
}

int TileMap::WorldToLocal(int worldCoord) const
{
    int local =worldCoord % CHUNK_SIZE;
    if (local < 0)
        local += CHUNK_SIZE;
    return local;
}

TileType TileMap::GenerateProceduralTile(int worldX, int worldY, int chunkX, int chunkY)
{
    if (IsProceduralRoad(worldX,worldY))
    {
        return TileType::ROAD;
    }
    if (IsProceduralWater(worldX, worldY))
    {
        return TileType::WATER;
    }
    return TileType::FLOOR;
}

bool TileMap::IsProceduralRoad(int worldX, int worldY) const
{
    const int ROAD_INTERVAL = 32;
    const int ROAD_WIDTH = 2;

    int xMod =((worldX % ROAD_INTERVAL)+ ROAD_INTERVAL)% ROAD_INTERVAL;

    bool verticalRoad =xMod < ROAD_WIDTH;

    int yMod =((worldY % ROAD_INTERVAL)+ ROAD_INTERVAL)% ROAD_INTERVAL;
    bool horizontalRoad =yMod < ROAD_WIDTH;
    return verticalRoad ||horizontalRoad;
}

bool TileMap::IsRoadConnectionFromMap(int worldX, int worldY) const
{
    if (worldX >= m_width)
    {
        if (worldX == m_width)
        {
            for (int y = 0;y < m_height;y++)
            {
                if (y == worldY &&GetTile(m_width - 1,y) == TileType::ROAD)
                {
                    return true;
                }
            }
        }
    }


    if (worldY >= m_height)
    {
        if (worldY == m_height)
        {
            for (int x = 0; x < m_width; x++)
            {
                if (x == worldX && GetTile(x, m_height - 1) == TileType::ROAD)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

unsigned int TileMap::HashCoords(int x, int y, unsigned int seed) const
{
    unsigned int h = (unsigned int)(x * 374761393 + y * 668265263 + (int)seed * 2246822519u);
    h = (h ^ (h >> 13)) * 1274126177u;
    h = h ^ (h >> 16);
    return h;
}

float TileMap::HashFloat01(int x, int y, unsigned int seed) const
{
    return (HashCoords(x, y, seed) % 10000) / 10000.0f;
}

bool TileMap::GetLakeInfoForRegion(int regionX, int regionY, MathEngine::Vector2& outCenter, float& outRadius) const
{
    float chanceRoll = HashFloat01(regionX, regionY, 999);
    if (chanceRoll >= LAKE_CHANCE)
        return false;   // 이 지역엔 호수 없음

    // 지역 안 랜덤한 위치에 중심점 배치
    float offsetX = HashFloat01(regionX, regionY, 111) * LAKE_REGION_SIZE;
    float offsetY = HashFloat01(regionX, regionY, 222) * LAKE_REGION_SIZE;

    outCenter.x = regionX * LAKE_REGION_SIZE + offsetX;
    outCenter.y = regionY * LAKE_REGION_SIZE + offsetY;

    outRadius = LAKE_MIN_RADIUS + HashFloat01(regionX, regionY, 333) * (LAKE_MAX_RADIUS - LAKE_MIN_RADIUS);
    return true;
}

bool TileMap::IsProceduralWater(int worldX, int worldY) const
{
    int regionX = (worldX >= 0) ? worldX / LAKE_REGION_SIZE : (worldX - LAKE_REGION_SIZE + 1) / LAKE_REGION_SIZE;
    int regionY = (worldY >= 0) ? worldY / LAKE_REGION_SIZE : (worldY - LAKE_REGION_SIZE + 1) / LAKE_REGION_SIZE;

    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            MathEngine::Vector2 center;
            float radius = 0.0f;
            if (!GetLakeInfoForRegion(regionX + dx, regionY + dy, center, radius))
                continue;

            float distX = worldX - center.x;
            float distY = worldY - center.y;
            float dist = sqrtf(distX * distX + distY * distY);

            // ★ 각도에 따라 반지름을 살짝 흔들어서 울퉁불퉁한 자연스러운 해안선 만들기
            float angle = atan2f(distY, distX);
            float wobble = 1.0f + 0.15f * sinf(angle * 5.0f + center.x * 0.7f)
                + 0.10f * sinf(angle * 9.0f + center.y * 0.5f);
            float effectiveRadius = radius * wobble;

            if (dist < effectiveRadius)
                return true;
        }
    }
    return false;
}


//D2D1_RECT_F TileMap::GetPathSrcRec(int bitmask) const
//{
//    const TileCoord& coord = m_pathTileMap[bitmask];
//    float left = (float)(coord.col * PATH_TILE_W);
//    float top = (float)(coord.row * PATH_TILE_H);
//    return D2D1::RectF(left, top, left + PATH_TILE_W, top + PATH_TILE_H);
//}