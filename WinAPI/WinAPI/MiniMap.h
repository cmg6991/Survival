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

	void RenderTargetArrows(ID2D1DeviceContext* context,const std::vector<GameObject*>& objects);
	void RenderTargetArrow(ID2D1DeviceContext* context,const MathEngine::Vector2& direction,const D2D1::ColorF& color,bool outside);

	MathEngine::Vector2 GetDiamondEdgePosition(const MathEngine::Vector2& direction) const;
private:
	float m_centerX;
	float m_centerY;

	float m_width;
	float m_height;

	float m_tileWidth;
	float m_tileHeight;

	float m_frameWidth = 350.0f;
	float m_frameHeight = 200.f;
};

