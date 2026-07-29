#pragma once
#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

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

    void Render(Graphics& graphics,ResourceManager* resourceManager);

    bool IsWall(int tileX, int tileY);

private:
    static const int WIDTH = 20;
    static const int HEIGHT = 20;

    Tile m_tiles[HEIGHT][WIDTH];
};

