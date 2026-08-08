#include "Bullet.h"
#include "GameObject.h"
#include "Transform.h"
#include "CollisionManager.h"

Bullet::Bullet(const MathEngine::Vector2& dir, float speed, float range, int damage)
	:m_dir(dir), m_speed(speed),m_range(range), m_damage(damage)
{
}

Bullet::~Bullet()
{
}

void Bullet::Init()
{
	m_transform = static_cast<Transform*>(m_gameObject->GetElement(ElementType::Transform));
}

void Bullet::FixedUpdate()
{
}

void Bullet::Update(float deltaTime)
{
    if (m_isDead) return;

    MathEngine::Vector2 current = m_transform->GetPostion();
    MathEngine::Vector2 moveAmount = m_dir * m_speed * deltaTime;
    MathEngine::Vector2 nextPos = current + moveAmount;

    m_traveled += moveAmount.Magnitude();

    // 사거리 초과 -> 소멸
    if (m_traveled >= m_range)
    {
        m_isDead = true;
        return;
    }

    // 벽 충돌 체크 (타일 좌표 반올림)
    int tileX = (int)round(nextPos.x);
    int tileY = (int)round(nextPos.y);
    if (m_collisionManager != nullptr && m_collisionManager->IsBlocked(tileX, tileY))
    {
        m_isDead = true;
        return;
    }

    m_transform->SetPosition(nextPos);
}

void Bullet::LateUpdate()
{
}

void Bullet::PreRender()
{
}

void Bullet::Render(ID2D1DeviceContext* context)
{
}

void Bullet::PostRender(ID2D1DeviceContext* context)
{
}

void Bullet::Release()
{
}

ElementType Bullet::GetElementType() const
{
	return ElementType::Bullet;
}

GameObject* Bullet::GetGameObject() const
{
	return m_gameObject;
}

void Bullet::SetGameObject(GameObject* gameObject)
{
    m_gameObject = gameObject;
}
