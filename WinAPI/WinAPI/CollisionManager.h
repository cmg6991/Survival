#pragma once
#include "pch.h"

class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	void Init();

	bool IsBlocked(int x, int y);
	void SetBlocked(int x, int y, bool blocked);


	void RenderDebug(Gdiplus::Graphics& graphics);

private:
	static const int WIDTH = 100;
	static const int HEIGHT = 100;

	bool m_blockMap[HEIGHT][WIDTH];
};

