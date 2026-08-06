#pragma once

#include <windows.h>
#include "ElementBase.h"
#include "../MathEngine/Vector2.h"
#include "Inventory.h"

class TileMap;
class Transform;
class CollisionManager;
class Animator;
class SpriteRenderer;
class Weapon;

class Player : public ElementBase
{
public:
	Player();
    ~Player();

	virtual void Init() override;

	virtual void FixedUpdate() override;
	virtual void Update(float deltaTime) override;
	virtual void LateUpdate() override;
	 
	virtual void PreRender() override;
	virtual void Render(ID2D1DeviceContext* context) override;
	virtual void PostRender(ID2D1DeviceContext* context) override;

	virtual void Release() override;

	virtual ElementType GetElementType() const override;

	virtual GameObject* GetGameObject() const override;
	virtual void SetGameObject(GameObject* gameObject) override;

	Transform* GetTransform() { return m_transform; }
	void SetTransform(Transform* val) { m_transform = val; }

	void SetTileMap(TileMap* map) { m_tileMap = map; }
	void SetCollisionManager(CollisionManager* collisionManager) { m_collisionManager = collisionManager; }

	Inventory* GetInventory() { return &m_inventory; }

	//string GetEquippedWeapon() const { return m_equippedWeapon; }

	//void EquipWeapon(string weaponID);
	void Attack();

	void SetWeapon(Weapon* weapon) { m_weapon = weapon; }
	Weapon* GetWeapon() { return m_weapon; }

private:
	bool m_isAutoMoving = false;

	//string m_equippedWeapon;
private:
	Transform* m_transform;
	float m_moveSpeed;
	MathEngine::Vector2 m_targetPos;
	//MathEngine::Vector2 m_debugTarget;

	TileMap* m_tileMap;
	CollisionManager* m_collisionManager;
	Animator* m_animator;
	SpriteRenderer* m_sprite;

	Inventory m_inventory = Inventory(10, 50);
	Weapon* m_weapon;
};

