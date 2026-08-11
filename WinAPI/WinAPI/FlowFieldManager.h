#pragma once

#include "pch.h"
#include "../MathEngine/Vector2.h"

class CollisionManager;

class FlowFieldManager
{
public:
	FlowFieldManager();
	~FlowFieldManager();

	void Init(CollisionManager* collisionManager, int width, int height);
	void Recompute(int playerTileX, int playerTileY);

	MathEngine::Vector2 GetDirection(int tileX, int tileY) const;

private:
	int Index(int x, int y) const { return y * m_width + x; }
	bool InBounds(int x, int y) const { return x >= 0 && x < m_width && y >= 0 && y < m_height; }
private:
	int m_width;
	int m_height;
	CollisionManager* m_collisionManager;

	std::vector<int> m_distance;
	std::vector<MathEngine::Vector2> m_direction;
};

