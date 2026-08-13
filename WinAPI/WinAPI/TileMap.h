#pragma once
#include <windows.h>
#include "Graphics.h"
#include "pch.h"

class ResourceManager;

enum class TileType
{
    FLOOR,
    ROAD,
    GRASS,
    WATER,
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
};

class TileMap
{
public:
    void Init();

    void LoadFromMapData(const vector<string>& mapData);
    void Render(ID2D1DeviceContext* context, ResourceManager* resourceManager);


    bool IsType(int x, int y, TileType type) const;
    TileType GetTile(int x, int y) const;
    void SetTile(int x, int y, TileType type);

private:
    int GetPathBitmask(int x, int y,TileType type) const;
    D2D1_RECT_F GetSrcRect(const AutotileConfig& config, int bitmask) const;

private:
    vector<vector<TileType>> m_tiles;

    int m_width;
    int m_height;

    unordered_map<TileType, string> m_tileImageKeys;       // 일반 바닥 타입 -> 이미지 키
    unordered_map<TileType, AutotileConfig> m_autotiles;
};