#include "TileManager.h"

void TileManager::Init(ID2D1DeviceContext* context)
{
	D2D1_SIZE_U size = context->GetPixelSize();
	m_tileWidth = size.width;
	m_tileHeight = size.height;
}

MathEngine::Vector2 TileManager::TileToScreen(const MathEngine::Vector2& tilePos)
{
	float x = (tilePos.x - tilePos.y) * (m_tileWidth * 0.5f);
	float y = (tilePos.x + tilePos.y) * (m_tileHeight * 0.5f);

	return { x,y };
}

MathEngine::Vector2 TileManager::ScreenToTile(const MathEngine::Vector2& screenPos)
{

	float tileX = (screenPos.x / (m_tileWidth * 0.5f) + screenPos.y / (m_tileHeight * 0.5f)) * 0.5f;
	float tileY = (screenPos.y / (m_tileHeight * 0.5f) - screenPos.x / (m_tileWidth * 0.5f)) * 0.5f;
	
	return { tileX,tileY };
}
