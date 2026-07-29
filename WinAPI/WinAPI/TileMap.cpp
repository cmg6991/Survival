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

            /*if (x == 0 ||y == 0 || x == WIDTH - 1 ||y == HEIGHT - 1)
            {
                m_tiles[y][x].tileType = TileType::WALL;
            }*/
        }
    }
}

void TileMap::Render(Graphics& graphics,ResourceManager* resourceManager)
{
    int TILE_W = TileManager::GetInstance().GetTileWidth();
    int TILE_H = TileManager::GetInstance().GetTileHeight();

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen({ (float)x, (float)y });

            int screenX = screen.x - CameraManager::GetInstance().GetX();
            int screenY =screen.y - CameraManager::GetInstance().GetY();

            Bitmap* bitmap = nullptr;

            if (m_tiles[y][x].tileType == TileType::FLOOR)
            {
                bitmap = resourceManager->GetImage("Tile_W");
            }

            DrawBitmap(graphics,bitmap, screenX,screenY);
        }
    }
}

