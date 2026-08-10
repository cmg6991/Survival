#include "Monster.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animator.h"

Monster::Monster(int maxHealth) :m_health(maxHealth)
{
}

Monster::~Monster()
{
}

void Monster::Init()
{
	m_transform = static_cast<Transform*>(m_gameObject->GetElement(ElementType::Transform));
	m_animator = static_cast<Animator*>(m_gameObject->GetElement(ElementType::Animator));

	if (m_animator != nullptr)
	{
		// 방향 구분 없이 제자리 애니메이션 하나만 반복 재생
		// (column: 프레임 개수, row: 몇 번째 줄, frameTime: 프레임당 시간)
		// 실제 몬스터 스프라이트시트 프레임 수/줄 번호에 맞게 조정해주세요
		m_animator->SetCellSize(46, 33);
		m_animator->SetAnimation(6, 0, 0.15f);
	}
}

void Monster::FixedUpdate()
{
}

void Monster::Update(float deltaTime)
{
}

void Monster::LateUpdate()
{
}

void Monster::PreRender()
{
}

void Monster::Render(ID2D1DeviceContext* context)
{
}

void Monster::PostRender(ID2D1DeviceContext* context)
{
}

void Monster::Release()
{
}

ElementType Monster::GetElementType() const
{
	return ElementType::Monster;
}

GameObject* Monster::GetGameObject() const
{
	return m_gameObject;
}

void Monster::SetGameObject(GameObject* gameObject)
{
	m_gameObject = gameObject;
}

void Monster::TakeDamage(int amount)
{
	m_health -= amount;
	if (m_health < 0) m_health = 0;
}
