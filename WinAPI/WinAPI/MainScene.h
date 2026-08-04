#pragma once
#include "Scene.h"
#include "InteractType.h"
#include <unordered_set>

class TileMap;
class ResourceManager;
class CollisionManager;
class Player;
class Interactable;

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
	void SaveGame();

private:
	void RegisterTileHandlers();
	void CreateWall(float x, float y, const string& imageName);
	void CreateInteractable(float x, float y, InteractType type, const string& imageKey);
	void CreateItemPickUp(float x, float y, const string& itemId, int count);

	void CheckItemPickUps();
	string MakeItemPositionId(float x, float y);
	Interactable* FindNearByInteractable();
	void OnInteract(Interactable* target);

	void LoadMap(const vector<string>& mapData);
	//void RenderUI(ID2D1DeviceContext* context);

private:
	string InteractTypeToStationString(InteractType type);
private:
	TileMap* m_tileMap;
	ResourceManager* m_resourceManager;
	CollisionManager* m_collisionManager;
	Player* m_player;

	unordered_map<char, function<void(float, float)>> m_tileHandlers;
	unordered_set<string> m_collectedItemsIds;

	bool m_isCraftingOpen = false;
	InteractType m_currentCraftingType = InteractType::CampFire;
	int m_selectedRecipeIndex = 0;

	//wstring m_lastMessage;
};

