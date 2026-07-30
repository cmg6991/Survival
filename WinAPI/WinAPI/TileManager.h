#pragma once
#include "Singleton.h"
#include "pch.h"
#include "Vector2.h"
#include "Graphics.h"

class TileManager :public Singleton<TileManager>
{
public:
	void Init(ID2D1Bitmap* tileImage);

	MathEngine::Vector2 TileToScreen(const MathEngine::Vector2& tilePos);
	MathEngine::Vector2 ScreenToTile(const MathEngine::Vector2& screenPos);

	int GetTileWidth() { return m_tileWidth; }
	int GetTileHeight() { return m_tileHeight; }

private:
	TileManager() : m_tileWidth(96), m_tileHeight(48)  {}
	~TileManager(){}

private:
	int m_tileWidth;
	int m_tileHeight;

	friend class Singleton<TileManager>;
};

