#include "Fire.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Animator.h"

Fire::Fire()
{
}

Fire::~Fire()
{
}

void Fire::Init()
{
	m_sprite =static_cast<SpriteRenderer*>(m_gameObject->GetElement(ElementType::SpriteRenderer));
	m_animator =static_cast<Animator*>(m_gameObject->GetElement(ElementType::Animator));

	// 처음에는 불 꺼져 있음
	SetLit(false);
}

void Fire::FixedUpdate()
{
}

void Fire::Update(float deltaTime)
{
}

void Fire::LateUpdate()
{
}

void Fire::PreRender()
{
}

void Fire::Render(ID2D1DeviceContext* context)
{
}

void Fire::PostRender(ID2D1DeviceContext* context)
{
}

void Fire::Release()
{
	m_sprite = nullptr;
	m_animator = nullptr;
	m_gameObject = nullptr;
}

ElementType Fire::GetElementType() const
{
	return ElementType::Fire;
}

GameObject* Fire::GetGameObject() const
{
	return m_gameObject;
}

void Fire::SetGameObject(GameObject* gameObject)
{
	m_gameObject = gameObject;
}

void Fire::SetLit(bool lit)
{
	m_isLit = lit;

	if (!m_sprite)
		return;

	if (!m_isLit)
	{
		m_sprite->SetVisible(false);
		return;
	}

	if (m_animator)
	{
		m_animator->SetCellSize(32, 32);
		m_animator->SetAnimation(8, 0, 0.2f);

		// 첫 프레임을 즉시 지정
		m_sprite->SetFrameRect(
			0,
			0,
			32,
			32
		);
	}

	m_sprite->SetVisible(true);
}
