#include "Monster.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animator.h"
#include "CollisionManager.h"
#include "FlowFieldManager.h"
#include "TileMap.h"
#include "SpriteRenderer.h"
#include "ColliderComponent.h"
#include "SoundManager.h"

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
    m_collider = static_cast<ColliderComponent*>(m_gameObject->GetElement(ElementType::Collider));
    m_sprite = static_cast<SpriteRenderer*>(m_gameObject->GetElement(ElementType::SpriteRenderer));
    if (m_animator != nullptr)
	{
		m_animator->SetCellSize(46, 33);
		m_animator->SetAnimation(6, 0, 0.15f);
	}
}

void Monster::FixedUpdate()
{
}

void Monster::Update(float deltaTime)
{
    if (m_isDying)
    {
        m_deathTimer -= deltaTime;

        // ★ 남은 시간 비율에 따라 알파를 1.0 -> 0.0으로 선형 보간
        float t = m_deathTimer / m_deathDuration;
        if (t < 0.0f) t = 0.0f;

        if (m_sprite != nullptr)
            m_sprite->SetAlpha(t);

        return;
    }

    if (IsDead()) return;

    if (m_damageCooldownTimer > 0.0f)
        m_damageCooldownTimer -= deltaTime;

    //if (IsDead()) return;

    //if (m_damageCooldownTimer > 0.0f)
    //    m_damageCooldownTimer -= deltaTime;

    //if (m_target == nullptr || m_transform == nullptr)
    //    return;

    //MathEngine::Vector2 current = m_transform->GetPostion();
    //MathEngine::Vector2 targetPos = m_target->GetPostion();
    //float distToPlayer = (targetPos - current).Magnitude();

    //if (distToPlayer <= 0.1f)
    //    return;

    //  MathEngine::Vector2 dir;

    //  if (m_flowField != nullptr)
    //  {
    //      int tileX = (int)round(current.x);
    //      int tileY = (int)round(current.y);
    //      dir = m_flowField->GetDirection(tileX, tileY);
    //  }

    //  if (dir.Magnitude() < 0.01f)
    //  {
    //      dir = (targetPos - current).Normalize();
    //  }
    //  else
    //  {
    //      dir = dir.Normalize();
    //  }

    //  dir += m_separation * 1.5f;

    //  MathEngine::Vector2 obstacleAvoid(0.0f, 0.0f);

    //  if (m_collider != nullptr)
    //  {
    //      obstacleAvoid = m_collider->GetAvoidVector(current, 0.9f, m_targetCollider);
    //  }
    //  dir += obstacleAvoid * 1.5f;
    //  if (dir.Magnitude() > 0.01f)
    //  {
    //      dir = dir.Normalize();
    //  }

    //  MathEngine::Vector2 nextPos = current + dir * m_moveSpeed * deltaTime;

    //  int nextTileX = (int)round(nextPos.x);
    //  int nextTileY = (int)round(nextPos.y);

    //  bool blockedByTile = (m_collisionManager != nullptr && m_collisionManager->IsBlocked(nextTileX, nextTileY));
    //  bool blockedByWater =(m_tileMap != nullptr &&m_tileMap->IsWater(nextTileX, nextTileY));

    //  bool blockedByPhysics = false;
    //  //if (!blockedByTile && m_physicsWorld != nullptr)
    //  //{
    //  //    PhysicsEngine::CircleCollider testCollider(0.f, 0.f, 0.4f);
    //  //    testCollider.center = nextPos;
    //  //    blockedByPhysics = m_physicsWorld->IsColliderBlocked(testCollider, m_targetPhysicsObject,true);
    //  //    m_physicsWorld->PushDynamicObjects(testCollider, current, 0.4f, m_targetPhysicsObject);
    //  //}
    //  if (!blockedByTile && !blockedByWater && m_collider != nullptr)
    //  {
    //      blockedByPhysics = m_collider->IsPositionBlocked(nextPos, 0.4f, m_targetCollider, true);
    //      m_collider->PushNearbyDynamics(nextPos, 0.4f, current, m_targetCollider);
    //  }

    //  if (!blockedByTile && !blockedByWater && !blockedByPhysics)
    //  {
    //      m_transform->SetPosition(nextPos);
    //      return;
    //  }

    //  if (blockedByWater)
    //  {
    //      return;
    //  }

    //  const float tryAngles[] = { 30.0f, -30.0f, 60.0f, -60.0f, 90.0f, -90.0f };
    //  for (float angleDeg : tryAngles)
    //  {
    //      float rad = angleDeg * 3.14159265f / 180.0f; 
    //      float cosA = cosf(rad);
    //      float sinA = sinf(rad);

    //      MathEngine::Vector2 altDir;
    //      altDir.x = dir.x * cosA - dir.y * sinA;
    //      altDir.y = dir.x * sinA + dir.y * cosA;

    //      MathEngine::Vector2 altPos = current + altDir * m_moveSpeed * deltaTime;

    //      int altTileX = (int)round(altPos.x);
    //      int altTileY = (int)round(altPos.y);
    //      bool altBlockedByTile = (m_collisionManager != nullptr && m_collisionManager->IsBlocked(altTileX, altTileY));
    //      bool altBlockedByWater =(m_tileMap != nullptr &&m_tileMap->IsWater(altTileX,altTileY));
    //      bool altBlockedByPhysics = false;
    //      //if (!altBlockedByTile && m_physicsWorld != nullptr)
    //      //{
    //      //    PhysicsEngine::CircleCollider testCollider2(0.f, 0.f, 0.4f);
    //      //    testCollider2.center = altPos;
    //      //    altBlockedByPhysics = m_physicsWorld->IsColliderBlocked(testCollider2,m_targetPhysicsObject,true);
    //      //    m_physicsWorld->PushDynamicObjects(testCollider2, current, 0.4f, m_targetPhysicsObject);
    //      //}
    //      if (!altBlockedByTile && !altBlockedByWater && m_collider != nullptr)
    //      {
    //          altBlockedByPhysics = m_collider->IsPositionBlocked(altPos, 0.4f, m_targetCollider, true);
    //          m_collider->PushNearbyDynamics(altPos, 0.4f, current, m_targetCollider);
    //      }

    //      if (!altBlockedByTile && !altBlockedByWater && !altBlockedByPhysics)
    //      {
    //          m_transform->SetPosition(altPos);
    //          return;
    //      }
    //  }
    // 
    // 

    //MathEngine::Vector2 flowDir(0.0f, 0.0f);

    //if (m_flowField != nullptr)
    //{
    //    int tileX = (int)round(current.x);
    //    int tileY = (int)round(current.y);

    //    flowDir = m_flowField->GetDirection(tileX, tileY);
    //}

    //// FlowField 방향이 없으면 플레이어 직접 추적
    //if (flowDir.Magnitude() < 0.01f)
    //{
    //    flowDir = (targetPos - current).Normalize();
    //}
    //else
    //{
    //    flowDir = flowDir.Normalize();
    //}

    //MathEngine::Vector2 dir = flowDir + m_separation * 1.0f;

    //if (m_avoidTimer > 0.0f)
    //{
    //    m_avoidTimer -= deltaTime;

    //    dir += m_avoidDirection * 0.8f;
    //}

    //if (dir.Magnitude() > 0.01f)
    //{
    //    dir = dir.Normalize();
    //}

    //MathEngine::Vector2 nextPos = current + dir * m_moveSpeed * deltaTime;

    //int nextTileX = (int)round(nextPos.x);
    //int nextTileY = (int)round(nextPos.y);
    //// Tile 충돌
    //bool blockedByTile = (m_collisionManager != nullptr && m_collisionManager->IsBlocked(nextTileX, nextTileY));

    //// Physics 충돌
    //bool blockedByPhysics = false;

    //if (!blockedByTile && m_physicsWorld != nullptr)
    //{
    //    PhysicsEngine::CircleCollider testCollider(0.f, 0.f, 0.4f);

    //    testCollider.center = nextPos;

    //    blockedByPhysics = m_physicsWorld->IsColliderBlocked(testCollider);
    //}

    //if (!blockedByTile && !blockedByPhysics)
    //{
    //    m_transform->SetPosition(nextPos);

    //    return;
    //}

    //const float tryAngles[] =
    //{
    //    30.0f,
    //    -30.0f,
    //    60.0f,
    //    -60.0f,
    //    90.0f,
    //    -90.0f
    //};

    //for (float angleDeg : tryAngles)
    //{
    //    float rad = angleDeg * 3.14159265f / 180.0f;

    //    float cosA = cosf(rad);
    //    float sinA = sinf(rad);

    //    MathEngine::Vector2 altDir;

    //    altDir.x = dir.x * cosA - dir.y * sinA;

    //    altDir.y = dir.x * sinA + dir.y * cosA;

    //    if (altDir.Magnitude() > 0.01f)
    //        altDir = altDir.Normalize();


    //    MathEngine::Vector2 altPos = current + altDir * m_moveSpeed * deltaTime;

    //    int altTileX = (int)round(altPos.x);

    //    int altTileY = (int)round(altPos.y);

    //    bool altBlockedByTile = (m_collisionManager != nullptr && m_collisionManager->IsBlocked(altTileX, altTileY));

    //    bool altBlockedByPhysics = false;

    //    if (!altBlockedByTile &&
    //        m_physicsWorld != nullptr)
    //    {
    //        PhysicsEngine::CircleCollider testCollider(0.f, 0.f, 0.4f);

    //        testCollider.center = altPos;

    //        altBlockedByPhysics = m_physicsWorld->IsColliderBlocked(testCollider);
    //    }

    //    if (!altBlockedByTile && !altBlockedByPhysics)
    //    {
    //        m_avoidDirection = altDir;
    //        m_avoidTimer = 0.25f;
    //        m_transform->SetPosition(altPos);
    //        return;
    //    }
    //}
if (IsDead()) return;

    if (m_damageCooldownTimer > 0.0f)
        m_damageCooldownTimer -= deltaTime;

    if (m_target == nullptr || m_transform == nullptr)
        return;

    MathEngine::Vector2 current = m_transform->GetPostion();
    MathEngine::Vector2 targetPos = m_target->GetPostion();
    float distToPlayer = (targetPos - current).Magnitude();

    if (distToPlayer <= 0.1f)
        return;

    MathEngine::Vector2 dir;

    if (m_flowField != nullptr)
    {
        int tileX = (int)round(current.x);
        int tileY = (int)round(current.y);
        dir = m_flowField->GetDirection(tileX, tileY);
    }

    if (dir.Magnitude() < 0.01f)
        dir = (targetPos - current).Normalize();
    else
        dir = dir.Normalize();

    dir += m_separation * 1.5f;

    MathEngine::Vector2 obstacleAvoid(0.0f, 0.0f);
    if (m_collider != nullptr)
    {
        obstacleAvoid = m_collider->GetAvoidVector(current, 0.9f, m_targetCollider);
    }
    dir += obstacleAvoid * 1.5f;

    // ★ 회피 중이면, 방금 찾은 우회 방향을 잠깐 동안 계속 섞어줌 (부드러운 곡선)
    if (m_avoidTimer > 0.0f)
    {
        m_avoidTimer -= deltaTime;
        dir += m_avoidDirection * 0.8f;
    }

    if (dir.Magnitude() > 0.01f)
        dir = dir.Normalize();

    MathEngine::Vector2 nextPos = current + dir * m_moveSpeed * deltaTime;

    int nextTileX = (int)round(nextPos.x);
    int nextTileY = (int)round(nextPos.y);

    bool blockedByTile = (m_collisionManager != nullptr && m_collisionManager->IsBlocked(nextTileX, nextTileY));
    bool blockedByWater = (m_tileMap != nullptr && m_tileMap->IsWater(nextTileX, nextTileY));

    bool blockedByPhysics = false;
    if (!blockedByTile && !blockedByWater && m_collider != nullptr)
    {
        blockedByPhysics = m_collider->IsPositionBlocked(nextPos, 0.4f, m_targetCollider, true);
        m_collider->PushNearbyDynamics(nextPos, 0.4f, current, m_targetCollider);
    }

    if (!blockedByTile && !blockedByWater && !blockedByPhysics)
    {
        m_transform->SetPosition(nextPos);
        return;
    }

    if (distToPlayer > m_detailedAvoidRange)
    {
        return;
    }

    const float tryAngles[] = { 30.0f, -30.0f, 60.0f, -60.0f, 90.0f, -90.0f, 120.0f, -120.0f, 150.0f, -150.0f };
    for (float angleDeg : tryAngles)
    {
        float rad = angleDeg * 3.14159265f / 180.0f;
        float cosA = cosf(rad);
        float sinA = sinf(rad);

        MathEngine::Vector2 altDir;
        altDir.x = dir.x * cosA - dir.y * sinA;
        altDir.y = dir.x * sinA + dir.y * cosA;

        if (altDir.Magnitude() > 0.01f)
            altDir = altDir.Normalize();

        MathEngine::Vector2 altPos = current + altDir * m_moveSpeed * deltaTime;

        int altTileX = (int)round(altPos.x);
        int altTileY = (int)round(altPos.y);
        bool altBlockedByTile = (m_collisionManager != nullptr && m_collisionManager->IsBlocked(altTileX, altTileY));
        bool altBlockedByWater = (m_tileMap != nullptr && m_tileMap->IsWater(altTileX, altTileY));
        bool altBlockedByPhysics = false;
        if (!altBlockedByTile && !altBlockedByWater && m_collider != nullptr)
        {
            altBlockedByPhysics = m_collider->IsPositionBlocked(altPos, 0.4f, m_targetCollider, true);
            m_collider->PushNearbyDynamics(altPos, 0.4f, current, m_targetCollider);
        }

        if (!altBlockedByTile && !altBlockedByWater && !altBlockedByPhysics)
        {
            // ★ 이 방향을 잠깐 동안 기억해서, 다음 몇 프레임도 부드럽게 이 방향을 유지
            m_avoidDirection = altDir;
            m_avoidTimer = 0.25f;

            m_transform->SetPosition(altPos);
            return;
        }
    }

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
    if (m_isDying) return;

    SoundManager::GetInstance().PlaySFX("SlimeDamage", 0.5f);
    m_health -= amount;
    if (m_health < 0) m_health = 0;

    if (m_health <= 0 && !m_isDying)
    {
        ForceStartFadeOut();
    }
}

void Monster::Reset(int maxHealth)
{
    m_health = maxHealth;
    m_damageCooldownTimer = 0.0f;
    m_separation = MathEngine::Vector2(0.0f, 0.0f);
    m_avoidDirection = MathEngine::Vector2(0.0f, 0.0f);
    m_avoidTimer = 0.0f;
    m_target = nullptr;
    m_targetCollider = nullptr;

    if (m_animator != nullptr)
    {
        m_animator->SetAnimation(6, 0, 0.15f); // 애니메이션 처음부터
    }
}

void Monster::Reset(int maxHealth, int cellWidth, int cellHeight, int animColumn)
{
    m_health = maxHealth;
    m_damageCooldownTimer = 0.0f;
    m_separation = MathEngine::Vector2(0.0f, 0.0f);
    m_avoidDirection = MathEngine::Vector2(0.0f, 0.0f);
    m_avoidTimer = 0.0f;
    m_target = nullptr;
    m_targetCollider = nullptr;

    m_isDying = false;
    m_deathTimer = 0.0f;

    if (m_sprite != nullptr)
        m_sprite->SetAlpha(1.0f);

    if (m_collider != nullptr)
        m_collider->SetEnabled(true);

    if (m_animator != nullptr)
    {
        m_animator->SetCellSize(cellWidth, cellHeight);
        m_animator->SetAnimation(animColumn, 0, 0.15f);
    }
}

void Monster::ForceStartFadeOut()
{
    if (m_isDying) return;

    m_isDying = true;
    m_deathTimer = m_deathDuration;

    if (m_collider != nullptr)
        m_collider->SetEnabled(false);
}
