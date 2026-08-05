#include "Weapon.h"
#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Player.h"

Weapon::Weapon(string weaponId) 
	: m_weaponId(weaponId), m_transform(nullptr),m_sprite(nullptr), m_damage(10), m_attackRange(1.5f)
{
}

Weapon::~Weapon()
{
}

void Weapon::Init()
{
    m_transform =static_cast<Transform*>(m_gameObject->GetElement(ElementType::Transform));
    m_sprite =static_cast<SpriteRenderer*>(m_gameObject->GetElement(ElementType::SpriteRenderer));
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
