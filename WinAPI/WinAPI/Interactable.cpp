#include "Interactable.h"

Interactable::Interactable(InteractType type) : m_interactType(type)
{
}

void Interactable::Init()
{
}

void Interactable::FixedUpdate()
{
}

void Interactable::Update(float deltaTime)
{
}

void Interactable::LateUpdate()
{
}

void Interactable::PreRender()
{
}

void Interactable::Render(ID2D1DeviceContext* context)
{
}

void Interactable::PostRender(ID2D1DeviceContext* context)
{
}

void Interactable::Release()
{
}

ElementType Interactable::GetElementType() const
{
	return ElementType::Interactable;
}

GameObject* Interactable::GetGameObject() const
{
	return m_gameObject;
}

void Interactable::SetGameObject(GameObject* gameObject)
{
	m_gameObject = gameObject;
}
