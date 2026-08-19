#pragma once
#include "Scene.h"
#include "InteractType.h"
#include <unordered_set>
#include "../PhysicsEngine/PhysicsWorld.h"
#include "../PhysicsEngine/Collider.h"
#include "FlowFieldManager.h"
#include <map>

class TileMap;
class ResourceManager;
class CollisionManager;
class Player;
class Interactable;
class ColliderComponent;
class MonsterSpawner;
class MiniMap;
class ObjectSpawner;
class ResourceNode;
class SceneManager;

class MainScene : public Scene
{
public:
	MainScene(ResourceManager* resourceManager, SceneManager* sceneManager);
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

	void CreateItemPickUp(float x, float y, const string& itemId, int count, bool trackAsCollected = true);
private:
	void RegisterTileHandlers();

	void CreateWall(float x, float y, const string& imageName);
	void CreateInteractableWorkTable(float x, float y, InteractType type, const string& imageKey);
	void CreateInteractableFire(float x, float y, InteractType type, const string& imageKey);
	void CreateBullet(const MathEngine::Vector2& startPos, const MathEngine::Vector2& dir,
		int damage, float speed, float range);
	//void CreateMonster(float x, float y, int health);
	void CreateMonster(const string& monsterId, float x, float y);

	void EquipWeaponToPlayer(const string& weaponId, bool returnInven = true);
	void UnequipWeaponFromPlayer();

	void EquipShieldToPlayer(const string& shieldId);
	void UnequipShieldFromPlayer();

	void CheckItemPickUps();
	string MakeItemPositionId(float x, float y);
	Interactable* FindNearByInteractable();
	void OnInteract(Interactable* target);
	void HarvestResource(ResourceNode* resource);

	//총알 오브젝트 풀
	GameObject* AcquireBullet();
	void ReleaseBullet(GameObject* obj);

	//몬스터 오브젝트 풀
	GameObject* AcquireMonster(const string& monsterId);
	void ReleaseMonster(GameObject* obj,const string& monsterId);

	void ClearAllMonsters();

	void LoadMap(const vector<string>& mapData);

	void ApplyMeleeDamage(const MathEngine::Vector2& startPos, const MathEngine::Vector2& dir, int damage, float range);

	void CheckBullets();
	void CheckMonsters();
	void CheckAttackHitBoxes();
	void RenderAimLine(ID2D1DeviceContext* context);
	void UpdateMonsterSeparation();

	void UseItem(const string& itemId);
	void SpawnItemDrop(const MathEngine::Vector2& centerPos, const string& itemId, int count);

private:
	string InteractTypeToStationString(InteractType type);
	bool IsWall(const vector<string>& mapData, int x, int y);
	string GetWallImage(const vector<string>& mapData, int x, int y);
private:
	TileMap* m_tileMap;
	ResourceManager* m_resourceManager;
	CollisionManager* m_collisionManager;
	Player* m_player;
	FlowFieldManager m_flowField;
	MonsterSpawner* m_monsterSpawner;
	ObjectSpawner* m_objectSpawner;
	SceneManager* m_sceneManager;

	unordered_map<char, function<void(float, float)>> m_tileHandlers;
	unordered_set<string> m_collectedItemsIds;

	PhysicsEngine::PhysicsWorld* m_physicsWorld;
	
	//오브젝트 풀
	vector<GameObject*> m_bulletPool;
	map<string, vector<GameObject*>> m_monsterPool;

	int m_lastPlayerTileX = -9999;
	int m_lastPlayerTileY = -9999;

	MiniMap* m_miniMap;

	float m_flowFieldRecomputeTimer = 0.0f;
	const float m_flowFieldRecomputeInterval = 0.4f;

	const float m_monsterDespawnDistance = 20.0f;

	float m_playTime = 0.0f;
	int m_transientPickupCounter = 0;
};