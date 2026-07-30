#pragma once
#include <windows.h>
#include "Graphics.h"

enum class TileType
{
    FLOOR,
    WALL,
    PLAYER
};

struct Tile
{
    TileType tileType;

    int tileX;
    int tileY;
};

class ResourceManager;

class TileMap
{
public:
    void Init();

    void Render(ID2D1DeviceContext* context ,ResourceManager* resourceManager);

    bool IsWall(int tileX, int tileY);

private:
    static const int WIDTH = 100;
    static const int HEIGHT = 100;

    Tile m_tiles[HEIGHT][WIDTH];
};

