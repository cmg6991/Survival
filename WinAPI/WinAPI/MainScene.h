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

private:
	string InteractTypeToStationString(InteractType type);
private:
	TileMap* m_tileMap;
	ResourceManager* m_resourceManager;
	CollisionManager* m_collisionManager;
	Player* m_player;
	FlowFieldManager m_flowField;
	MonsterSpawner* m_monsterSpawner;

	unordered_map<char, function<void(float, float)>> m_tileHandlers;
	unordered_set<string> m_collectedItemsIds;

	PhysicsEngine::PhysicsWorld* m_physicsWorld;
	
	//오브젝트 풀
	vector<GameObject*> m_bulletPool;
	map<string, vector<GameObject*>> m_monsterPool;

	int m_lastPlayerTileX = -9999;
	int m_lastPlayerTileY = -9999;
};