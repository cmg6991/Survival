#pragma once
#include "ElementBase.h"

class SpriteRenderer;
class Animator;

class Fire : public ElementBase
{
public:
	Fire();
	~Fire();

	void Init() override;
	void FixedUpdate() override;
	void Update(float deltaTime) override;
	void LateUpdate() override;
	void PreRender() override;
	void Render(ID2D1DeviceContext* context) override;
	void PostRender(ID2D1DeviceContext* context) override;
	void Release() override;

	ElementType GetElementType() const override;

	GameObject* GetGameObject() const;
	void SetGameObject(GameObject* gameObject);

	void SetLit(bool lit);
	bool IsLit() const { return m_isLit; }

private:
	SpriteRenderer* m_sprite = nullptr;
	Animator* m_animator = nullptr;
	bool m_isLit = false;
};

