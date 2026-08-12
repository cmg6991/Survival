#pragma once

#include "pch.h"
#include "../MathEngine/Vector2.h"

class Player;
class ResourceManager;

class MiniMap
{
public:
	MiniMap();
	~MiniMap();

	void Init(Player* player, ResourceManager* resourceManager);
	void Render(ID2D1DeviceContext* context);

private:
	Player* m_player;
	ResourceManager* m_resourceManager;

	static const int MAP_WIDTH = 20;
	static const int MAP_HEIGHT = 20;

	MathEngine::Vector2 TileToMiniMap(float tileX,float tileY) const;
	void CreateDiamondGeometry(ID2D1Factory* factory,ID2D1PathGeometry** geometry);

private:
	float m_centerX;
	float m_centerY;

	float m_width;
	float m_height;

	float m_tileWidth;
	float m_tileHeight;
};

