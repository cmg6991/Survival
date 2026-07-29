#include "CameraManager.h"
#include "InputManager.h"
#include "Transform.h"
#include "TileManager.h"

void CameraManager::Init()
{
}

void CameraManager::Follow(Transform* transform)
{
    int TILE_W = TileManager::GetInstance().GetTileWidth();
    int TILE_H = TileManager::GetInstance().GetTileHeight();

    MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen( transform->GetPostion());

    m_x = screen.x - 960;
    m_y = screen.y - 540;
}
