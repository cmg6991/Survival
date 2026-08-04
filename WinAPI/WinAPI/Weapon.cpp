#include "Weapon.h"
#include "GameObject.h"

Weapon::Weapon(string weaponId) 
{
}

Weapon::~Weapon()
{
}

void Weapon::Init()
{
}

void Weapon::FixedUpdate()
{
}

void Weapon::Update(float deltaTime)
{
}

void Weapon::LateUpdate()
{
}

void Weapon::PreRender()
{
}

void Weapon::Render(ID2D1DeviceContext* context)
{
}

void Weapon::PostRender(ID2D1DeviceContext* context)
{
}

void Weapon::Release()
{
}

ElementType Weapon::GetElementType() const
{
	return ElementType::Weapon;
}

GameObject* Weapon::GetGameObject() const
{
	return m_gameObject;
}

void Weapon::SetGameObject(GameObject* gameObject)
{
	m_gameObject = gameObject;
}

void Weapon::Attack()
{
}
