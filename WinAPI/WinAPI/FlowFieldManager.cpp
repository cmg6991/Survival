#include "pch.h"
#include "FlowFieldManager.h"
#include "CollisionManager.h"

FlowFieldManager::FlowFieldManager() : m_width(0), m_height(0), m_collisionManager(nullptr)
{
}

FlowFieldManager::~FlowFieldManager()
{
}

void FlowFieldManager::Init(CollisionManager* collisionManager, int width, int height)
{
    m_collisionManager = collisionManager;
    m_width = width;
    m_height = height;
    m_distance.assign(width * height, INT_MAX);
    m_direction.assign(width * height, MathEngine::Vector2(0, 0));
}

void FlowFieldManager::Recompute(int playerTileX, int playerTileY)
{
    fill(m_distance.begin(), m_distance.end(), INT_MAX);

    if (!InBounds(playerTileX, playerTileY))
        return;

    queue<pair<int, int>> q;
    m_distance[Index(playerTileX, playerTileY)] = 0;
    q.push({ playerTileX, playerTileY });

    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };

    while (!q.empty())
    {
        auto [cx, cy] = q.front();
        q.pop();
        int curDist = m_distance[Index(cx, cy)];

        for (int i = 0; i < 4; i++)
        {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            if (!InBounds(nx, ny)) continue;
            if (m_collisionManager->IsBlocked(nx, ny)) continue;

            int idx = Index(nx, ny);
            if (m_distance[idx] > curDist + 1)
            {
                m_distance[idx] = curDist + 1;
                q.push({ nx, ny });
            }
        }
    }

    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            int idx = Index(x, y);
            if (m_distance[idx] == INT_MAX)
            {
                m_direction[idx] = MathEngine::Vector2(0, 0);
                continue;
            }

            int bestDist = m_distance[idx];
            int bestDx = 0, bestDy = 0;

            for (int i = 0; i < 4; i++)
            {
                int nx = x + dx[i];
                int ny = y + dy[i];
                if (!InBounds(nx, ny)) continue;

                int ndist = m_distance[Index(nx, ny)];
                if (ndist < bestDist)
                {
                    bestDist = ndist;
                    bestDx = dx[i];
                    bestDy = dy[i];
                }
            }

            m_direction[idx] = MathEngine::Vector2((float)bestDx, (float)bestDy);
        }
    }
}

MathEngine::Vector2 FlowFieldManager::GetDirection(int tileX, int tileY) const
{
    if (!InBounds(tileX, tileY)) return MathEngine::Vector2(0, 0);
    return m_direction[Index(tileX, tileY)];
}
