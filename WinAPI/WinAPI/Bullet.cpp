#include "Bullet.h"
#include "GameObject.h"
#include "Transform.h"
#include "CollisionManager.h"
#include "SpriteRenderer.h"
#include "TileManager.h"

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
    m_sprite =static_cast<SpriteRenderer*>(m_gameObject->GetElement(ElementType::SpriteRenderer));

    UpdateRotation();
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

void Bullet::Reset(const MathEngine::Vector2& dir, float speed, float range, int damage)
{
    m_dir = dir;
    m_speed = speed;
    m_range = range;
    m_damage = damage;
    m_traveled = 0.0f;
    m_isDead = false;
    UpdateRotation();
}

void Bullet::UpdateRotation()
{
    if (m_sprite == nullptr || m_transform == nullptr)
        return;

    MathEngine::Vector2 current = m_transform->GetPostion();

    MathEngine::Vector2 a =
        TileManager::GetInstance().TileToScreen(current);

    MathEngine::Vector2 b =
        TileManager::GetInstance().TileToScreen(current + m_dir);

    float angle = atan2f(
        b.y - a.y,
        b.x - a.x
    ) * 180.0f / 3.14159265f;

    m_sprite->SetRotation(angle);
}
