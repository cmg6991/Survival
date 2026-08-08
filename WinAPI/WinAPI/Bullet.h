#pragma once
#include "ElementBase.h"
#include "../MathEngine/Vector2.h"

class Transform;
class CollisionManager;

class Bullet : public ElementBase
{
public:
	Bullet(const MathEngine::Vector2& dir, float speed, float range, int damage);
	~Bullet();
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

	void SetCollisionManager(CollisionManager* cm) { m_collisionManager = cm; }
	bool IsDead() const { return m_isDead; }
	int GetDamage() const { return m_damage; }

private:
	Transform* m_transform = nullptr;
	CollisionManager* m_collisionManager = nullptr;

	MathEngine::Vector2 m_dir;
	float m_speed;
	float m_traveled = 0.0f;
	float m_range;
	int m_damage;
	bool m_isDead = false;
};

