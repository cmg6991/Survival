#include "AttackHitBox.h"
#include "GameObject.h"

AttackHitBox::AttackHitBox(int damage, float lifeTime) : m_damage(damage), m_timer(lifeTime)
{
}

AttackHitBox::~AttackHitBox()
{
}

void AttackHitBox::Init()
{
}

void AttackHitBox::FixedUpdate()
{
}

void AttackHitBox::Update(float deltaTime)
{
	m_timer -= deltaTime;
}

void AttackHitBox::LateUpdate()
{
}

void AttackHitBox::PreRender()
{
}

void AttackHitBox::Render(ID2D1DeviceContext* context)
{
}

void AttackHitBox::PostRender(ID2D1DeviceContext* context)
{
}

void AttackHitBox::Release()
{
}

ElementType AttackHitBox::GetElementType() const
{
	return ElementType::AttackHitBox;
}

GameObject* AttackHitBox::GetGameObject() const
{
	return m_gameObject;
}

void AttackHitBox::SetGameObject(GameObject* gameObject)
{
	m_gameObject = gameObject;
}
