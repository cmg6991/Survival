#pragma once
#include "ElementBase.h"
#include "pch.h"

class SpriteRenderer;
class Animator;
class Fire;

class CampFire : public ElementBase
{
public:
	CampFire():m_sprite(nullptr), m_animator(nullptr),m_fire(nullptr) {}
	~CampFire() {}

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

	bool IsLit() const { return m_isLit;}
	void AddFuel(int woodCount);
	float GetRemainingTime() const { return m_burnTimer; }

	void SetFire(Fire* fire) { m_fire = fire; }

private:
	bool m_isLit = false;
	float m_burnTimer = 0.f;
	const float m_burnTimePerWood = 30.0f;

	SpriteRenderer* m_sprite;
	Animator* m_animator;
	Fire* m_fire;
};

