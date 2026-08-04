#pragma once

#include "ElementBase.h"
#include "pch.h"

class Transform;
class SpriteRenderer;

class Weapon : public ElementBase
{
public:
	Weapon(string weaponId);
	~Weapon();

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

	void Attack();

	string GetWeaponId() { return m_weaponId; }

private:
	string m_weaponId;

	Transform* m_transform;
	SpriteRenderer* m_sprite;

	int m_damage;
	float m_attackRange;
};

