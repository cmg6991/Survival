#pragma once
#include "ElementBase.h"

class SpriteRenderer;

class Animator : public ElementBase
{
public:
	Animator();
	~Animator();
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

	void SetAnimation(int column, int row,float frameTime);

	void Play(int row, int frameCount, float frameTime);

private:
	SpriteRenderer* m_sprite;

	int m_frame;
	int m_column;
	int m_row;
	float m_timer;
	float m_frameTime;
	int m_frameCount = 1;
};

