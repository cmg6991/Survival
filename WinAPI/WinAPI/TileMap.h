#pragma once
#include <windows.h>
#include "Graphics.h"
#include "pch.h"
#include "../MathEngine/Vector2.h"
#include "ChunkType.h"

class ResourceManager;
class ObjectSpawner;

enum class TileType
{
    FLOOR,
    ROAD,
    GRASS,
    WATER,
    SNOW,
    STONE,
    END
};

struct TileCoord { int row = 0, col = 0; };

struct AutotileConfig
{
    string imageKey;
    int tileW = 64;
    int tileH = 32;
    TileCoord bitmaskMap[16];
    int bitN = 1;
    int bitE = 2;
    int bitS = 4;
    int bitW = 8;
    bool useDiagonalNeighbors = false;
};

class TileMap
{
public :
    static constexpr int CHUNK_SIZE = 16;
public:
    void Init();

    void LoadFromMapData(const vector<string>& mapData);
    void Render(ID2D1DeviceContext* context, ResourceManager* resourceManager);

    bool IsRoad(int x, int y) const;
    bool IsWater(int x, int y) const { return GetTile(x, y) == TileType::WATER; }
    bool IsType(int x, int y, TileType type) const;
    TileType GetTile(int x, int y) const;
    void SetTile(int x, int y, TileType type);
    void SetObjectSpawner(ObjectSpawner* spawner) { m_objectSpawner = spawner; }

public:
    ChunkType GetChunkBiome(int chunkX, int chunkY) const;
    ChunkType GetBiomeAt(float worldX, float worldY) const;

private:

    static const int BIOME_CELL_SIZE = 128;               // 지역 하나의 대략적 크기 (타일)
    static constexpr float BIOME_WOBBLE = 24.0f;           // 경계선 흔들림 폭
    static constexpr float LAKE_FILL_RADIUS_RATIO = 0.42f; // 호수가 셀 크기 대비 얼마나 크게 채워질지

    MathEngine::Vector2 GetBiomeSeedPoint(int cellX, int cellY) const;
    ChunkType GetBiomeTypeForCell(int cellX, int cellY) const;
private:
    int GetPathBitmask(int x, int y,TileType type) const;
    D2D1_RECT_F GetSrcRect(const AutotileConfig& config, int bitmask) const;

private:
    //chunk
    void EnsureChunk(int chunkX, int chunkY);
    void GenerateChunk(int chunkX,int chunkY);
    bool IsChunkGenerated(int chunkX,int chunkY) const;
    int WorldToChunk(int worldCoord) const;
    int WorldToLocal(int worldCoord) const;
    void UnloadFarChunks(int centerChunkX, int centerChunkY, int keepRadius);

    //tile
    TileType GenerateProceduralTile(int worldX,int worldY,int chunkX,int chunkY);
    bool IsProceduralRoad(int worldX,int worldY) const;
    bool IsRoadConnectionFromMap(int worldX,int worldY) const;

private:
    //물
    static const int LAKE_REGION_SIZE = 32;
    static constexpr float LAKE_CHANCE = 0.8f;      // 지역당 호수가 생길 확률
    static constexpr float LAKE_MIN_RADIUS = 4.0f;
    static constexpr float LAKE_MAX_RADIUS = 9.0f;

    unsigned int HashCoords(int x, int y, unsigned int seed) const;
    float HashFloat01(int x, int y, unsigned int seed) const;
    bool GetLakeInfoForRegion(int regionX, int regionY, MathEngine::Vector2& outCenter, float& outRadius) const;
    bool IsProceduralWater(int worldX, int worldY) const;
    bool IsProceduralSnow(int worldX, int worldY) const;
    bool IsProceduralStone(int worldX, int worldY) const;
private:
    vector<vector<TileType>> m_tiles;

    int m_width;
    int m_height;

    unordered_map<TileType, string> m_tileImageKeys;       // 일반 바닥 타입 -> 이미지 키
    unordered_map<TileType, AutotileConfig> m_autotiles;

    // 자동 생성된 Chunk
    unordered_set<long long> m_generatedChunks;
    unordered_map<long long, TileType> m_proceduralTiles;

    unordered_map<long long, pair<int, int>> m_activeChunkCoords;

    ObjectSpawner* m_objectSpawner = nullptr;
};