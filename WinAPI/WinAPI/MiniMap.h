#pragma once

#include "pch.h"
#include "../MathEngine/Vector2.h"

class Player;
class ResourceManager;
class GameObject;

class MiniMap
{
public:
	MiniMap();
	~MiniMap();

	void Init(Player* player, ResourceManager* resourceManager);
	void Render(ID2D1DeviceContext* context, const std::vector<GameObject*>& objects);

private:
	Player* m_player;
	ResourceManager* m_resourceManager;

	MathEngine::Vector2 TileToMiniMap(float tileX,float tileY) const;
	void CreateDiamondGeometry(ID2D1Factory* factory, ID2D1PathGeometry** geometry);
	void RenderObjects(ID2D1DeviceContext* context, const std::vector<GameObject*>& objects);
private:
	float m_centerX;
	float m_centerY;

	float m_width;
	float m_height;

	float m_tileWidth;
	float m_tileHeight;
};

