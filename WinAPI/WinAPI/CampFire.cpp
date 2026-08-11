#include "CampFire.h"
#include "GameObject.h"
#include "EnvironmentManager.h"
#include "Animator.h"
#include "SpriteRenderer.h"
#include "Fire.h"

void CampFire::Init()
{
	EnvironmentManager::GetInstance().RegisterCampFire(this);
}

void CampFire::FixedUpdate()
{
}

void CampFire::Update(float deltaTime)
{
    if (!m_isLit)
        return;

    m_burnTimer -= deltaTime;

    if (m_burnTimer <= 0.0f)
    {
        m_burnTimer = 0.0f;
        m_isLit = false;

        if (m_fire)
        {
            m_fire->SetLit(false);
        }
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
    if (m_fire)
        m_fire->SetLit(true);
}
