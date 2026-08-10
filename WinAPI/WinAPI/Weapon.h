#pragma once

#include "ElementBase.h"
#include "pch.h"
#include "../MathEngine/Vector2.h"

enum class WeaponType
{
	Melee,		//근거리
	Ranged		//원거리
};

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

	bool IsMuzzleFlashActive() const {return m_timeSinceFire < m_muzzleFlashDuration;
	}

	string GetWeaponId() { return m_weaponId; }
	Transform* GetTransform() { return m_transform; }

	void SetWeaponType(WeaponType type) { m_weaponType = type; }
	WeaponType GetWeaponType() const { return m_weaponType; }

	void SetBulletStat(float speed, float range, int damage)
	{
		m_bulletSpeed = speed;
		m_bulletRange = range;
		m_damage = damage;
	}

	void SetMeleeStat(float range, int damage) { m_attackRange = range; m_damage = damage; }

	// MainScene이 여기에 "총알 생성 함수"를 꽂아준다 (기존 SetOnWeaponEquip 패턴과 동일)
	void SetOnFire(function<void(const MathEngine::Vector2&, const MathEngine::Vector2&, int, float, float)> callback)
	{
		m_onFire = callback;
	}
	void SetOnMeleeAttack(function<void(const MathEngine::Vector2&, const MathEngine::Vector2&, int, float)> callback)
	{
		m_onMeleeAttack = callback;
	}
private:
	string m_weaponId;

	Transform* m_transform;
	SpriteRenderer* m_sprite;

	int m_damage;
	float m_attackRange;

	WeaponType m_weaponType = WeaponType::Melee;
	float m_bulletSpeed = 12.0f;
	float m_bulletRange = 8.0f;

	function<void(const MathEngine::Vector2& startPos,
		const MathEngine::Vector2& dir,
		int damage, float speed, float range)> m_onFire;
	function<void(const MathEngine::Vector2& startPos, const MathEngine::Vector2& dir,
		int damage, float range)> m_onMeleeAttack;

	float m_timeSinceFire = 999.0f;
	const float m_muzzleFlashDuration = 0.08f;
};

