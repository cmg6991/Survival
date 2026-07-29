#pragma once

#include "pch.h"
#include "GameObject.h"
#include "Graphics.h"

class TileMap;
class Player;
class ResourceManager;
class CollisionManager;
class Tree;

class GameEngine
{
public:
	GameEngine();
	~GameEngine();

	void Init(const HWND hwnd);
	void Update();
	void Render(ID2D1DeviceContext* context);
	void Release();

	void CreateWall(float x, float y, const string& imageName);

private:
	TileMap* m_tileMap;
	Player* m_player;
	ResourceManager* m_resourceManager;
	CollisionManager* m_collisionManager;
	Tree* m_tree;
	
	vector<GameObject*> m_objects;
};

