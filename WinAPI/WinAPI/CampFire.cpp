#include "CampFire.h"
#include "GameObject.h"
#include "EnvironmentManager.h"
#include "Animator.h"
#include "SpriteRenderer.h"

void CampFire::Init()
{
	EnvironmentManager::GetInstance().RegisterCampFire(this);
}

void CampFire::FixedUpdate()
{
}

void CampFire::Update(float deltaTime)
{
	if (!m_isLit) return;

	m_burnTimer -= deltaTime;

	if (m_burnTimer <= 0.0f)
	{
		m_burnTimer = 0.0f;
		m_isLit = false;
		//if (m_sprite)
		//	m_sprite->SetVisible(false);
		// 나중에 이미지 교체 자리
		// SpriteRenderer* sprite = static_cast<SpriteRenderer*>(m_gameObject->GetElement(ElementType::SpriteRenderer));
		// sprite->ChangeImage("CampFire_Off");
	}
}

void CampFire::LateUpdate()
{
}

void CampFire::PreRender()
{
}

void CampFire::Render(ID2D1DeviceContext* context)
{
}

void CampFire::PostRender(ID2D1DeviceContext* context)
{
}

void CampFire::Release()
{
	EnvironmentManager::GetInstance().UnregisterCampFire(this);
}

ElementType CampFire::GetElementType() const
{
	return ElementType::CampFire;
}

GameObject* CampFire::GetGameObject() const
{
	return m_gameObject;
}

void CampFire::SetGameObject(GameObject* gameObject)
{
	m_gameObject = gameObject;
}

void CampFire::AddFuel(int woodCount)
{
	m_burnTimer += woodCount * m_burnTimePerWood;
	m_isLit = true;
	//if (!m_isLit)
	//{
	//	m_isLit = true;

	//	if (m_sprite)
	//		m_sprite->SetVisible(true);

	//	if (m_animator)
	//		m_animator->SetAnimation(0, 4, 0.1f);
	//}
}
