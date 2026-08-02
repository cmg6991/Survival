#include "ItemPickUp.h"
#include "GameObject.h"
#include "Transform.h"

ItemPickUp::ItemPickUp(string itemId, int count)
	: m_itemId(itemId), m_count(count), m_transform(nullptr)
{
}

ItemPickUp::~ItemPickUp()
{
}

void ItemPickUp::Init()
{
	m_transform = static_cast<Transform*>(m_gameObject->GetElement(ElementType::Transform));
}

void ItemPickUp::FixedUpdate()
{
}

void ItemPickUp::Update(float deltaTime)
{
}

void ItemPickUp::LateUpdate()
{
}

void ItemPickUp::PreRender()
{
}

void ItemPickUp::Render(ID2D1DeviceContext* context)
{
}

void ItemPickUp::PostRender(ID2D1DeviceContext* context)
{
}

void ItemPickUp::Release()
{
}

ElementType ItemPickUp::GetElementType() const
{
	return ElementType::ItemPickUp;
}

GameObject* ItemPickUp::GetGameObject() const
{
	return m_gameObject;
}

void ItemPickUp::SetGameObject(GameObject* gameObject)
{
	m_gameObject = gameObject;
}
