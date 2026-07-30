#pragma once
#include "pch.h"
#include "Graphics.h"

class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	void Init();

	bool IsBlocked(int x, int y);
	void SetBlocked(int x, int y, bool blocked);


	void RenderDebug(ID2D1DeviceContext* context);

private:
	static const int WIDTH = 100;
	static const int HEIGHT = 100;

	bool m_blockMap[HEIGHT][WIDTH];
};

