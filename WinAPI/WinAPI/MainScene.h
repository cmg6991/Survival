#pragma once
#include "Scene.h"

class TileMap;
class ResourceManager;
class CollisionManager;
class Player;

class MainScene : public Scene
{
public:
	MainScene(ResourceManager* resourceManager);
	~MainScene();

	void Init() override;

	void FixedUpdate() override;
	void Update(float deltaTime) override;
	void LateUpdate() override;

	void PreRender() override;
	void Render(ID2D1DeviceContext* context) override;
	void PostRender(ID2D1DeviceContext* context) override;

	void Release() override;

private:
	void CreateWall(float x, float y, const string& imageName);
private:
	TileMap* m_tileMap;
	ResourceManager* m_resourceManager;
	CollisionManager* m_collisionManager;
	Player* m_player;

};

