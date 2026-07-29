#pragma once
#include "pch.h"

enum class ElementType
{
	Transform,
	Animator,
	Collider,
	Player,
	SpriteRenderer,
	Wall,
	End
};

class GameObject;

class ElementBase
{
public:
	virtual void Init() abstract;

	virtual void FixedUpdate() abstract;
	virtual void Update(float deltaTime) abstract;
	virtual void LateUpdate() abstract;

	virtual void PreRender() abstract;
	virtual void Render(Graphics& graphics) abstract;
	virtual void PostRender(HDC hdc) abstract;

	virtual void Release() abstract;

	virtual ElementType GetElementType() const abstract;

	virtual GameObject* GetGameObject() const abstract;
	virtual void SetGameObject(GameObject* gameObject) abstract;

	virtual ~ElementBase() {}

protected:
	ElementBase() : m_gameObject(nullptr) {}

	GameObject* m_gameObject;
};