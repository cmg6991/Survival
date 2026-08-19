#pragma once

#include <windows.h>
#include "ElementBase.h"
#include "../MathEngine/Vector2.h"
#include "Inventory.h"
#include "FishingController.h"

class TileMap;
class Transform;
class CollisionManager;
class Animator;
class SpriteRenderer;
class Weapon;
class ColliderComponent;

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

	void Attack();

	void SetWeapon(Weapon* weapon) { m_weapon = weapon; }
	Weapon* GetWeapon() { return m_weapon; }

	void SetArmedVisaul(const string& spriteKey);
	void ClearArmedVisual();

	void SetShield(const string& spriteKey, int defense);
	void ClearShield();
	int GetDefense() const { return m_shieldDefense; }

	void SetEquippedShieldId(const string& id) { m_equippedShieldId = id; }
	string GetEquippedShieldId() const { return m_equippedShieldId; }

	int GetMaxHealth() const { return m_maxHealth; }
	int GetHealth() const { return m_currentHealth; }
	bool IsDead() const { return m_currentHealth <= 0;}
	void TakeDamage(int rawDamage);

	void Heal(int amount);

	Animator* GetAnimator() { return m_animator; }

	void SetGlowRing(bool equipped) { m_hasGlowRing = equipped; }
	bool HasGlowRing() const { return m_hasGlowRing; }

private:
	bool m_isAutoMoving = false;

	void UpdateSpriteState();
	void ApplyFacing(const MathEngine::Vector2& faceDir);
	bool m_isMoving = false;
	bool m_facingRight = true;
private:
	Transform* m_transform;
	float m_moveSpeed;
	MathEngine::Vector2 m_targetPos;

	TileMap* m_tileMap;
	CollisionManager* m_collisionManager;
	Animator* m_animator;
	SpriteRenderer* m_sprite;
	ColliderComponent* m_collider = nullptr;
	FishingController m_fisingController;

	Inventory m_inventory = Inventory(10, 50);
	Weapon* m_weapon;

	string m_currentArmedSprite = "";
	string m_lastAppliedSprite = "";
	string m_shieldSpriteKey = "";
	int m_shieldDefense = 0;
	string m_equippedShieldId = "";

private:
	bool m_isAttacking = false;
	float m_attackTimer = 0.0f;
	float m_attackDuration = 0.3f;

private:
	int m_maxHealth = 100;
	int m_currentHealth = 100;
	bool m_isDying = false;

	bool m_hasGlowRing = false;
	//PhysicsEngine::PhysicsWorld* m_physicsWorld = nullptr;
	//PhysicsEngine::Object* m_selfPhysicsObject = nullptr;
};