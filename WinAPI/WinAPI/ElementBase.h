#pragma once
#include "pch.h"
#include "Graphics.h"

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
	virtual void Render(ID2D1DeviceContext* context) abstract;
	virtual void PostRender(ID2D1DeviceContext* context) abstract;

	virtual void Release() abstract;

	virtual ElementType GetElementType() const abstract;

	virtual GameObject* GetGameObject() const abstract;
	virtual void SetGameObject(GameObject* gameObject) abstract;

	virtual ~ElementBase() {}

protected:
	ElementBase() : m_gameObject(nullptr) {}

	GameObject* m_gameObject;
};