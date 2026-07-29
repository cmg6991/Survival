#include "CollisionManager.h"
#include "CameraManager.h"

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
        return true;
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

void CollisionManager::RenderDebug(Graphics& graphics)
{
    Pen pen(Color(255, 255, 0, 0), 2);

    const int TILE_W = 64;
    const int TILE_H = 32;

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (!m_blockMap[y][x])
                continue;

            float isoX = (x - y) * (TILE_W / 2.0f);
            float isoY = (x + y) * (TILE_H / 2.0f);

            int screenX = (int)(isoX - CameraManager::GetInstance().GetX());
            int screenY = (int)(isoY - CameraManager::GetInstance().GetY());

            Point pts[4] =
            {
                Point(screenX,              screenY - TILE_H / 2),
                Point(screenX + TILE_W / 2, screenY),
                Point(screenX,              screenY + TILE_H / 2),
                Point(screenX - TILE_W / 2, screenY)
            };

            graphics.DrawPolygon(&pen, pts, 4);
        }
    }
}