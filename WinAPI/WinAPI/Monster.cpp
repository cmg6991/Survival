#include "Monster.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animator.h"
#include "CollisionManager.h"
#include "FlowFieldManager.h"
//#include "../PhysicsEngine/PhysicsWorld.h"
//#include "../PhysicsEngine/CircleCollider.h"
#include "ColliderComponent.h"

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
      {
          dir = (targetPos - current).Normalize();
      }
      else
      {
          dir = dir.Normalize();
      }

      dir += m_separation * 1.5f;

      MathEngine::Vector2 obstacleAvoid(0.0f, 0.0f);
      //if (m_physicsWorld != nullptr)
      //{
      //    obstacleAvoid = m_physicsWorld->GetPushAwayVector(current, 0.4f, 0.9f, m_targetPhysicsObject);
      //    // 0.9f = 이 반경 안에 있는 장애물이면 밀어냄 (몬스터 반지름 + 여유폭)
      //}
      if (m_collider != nullptr)
      {
          obstacleAvoid = m_collider->GetAvoidVector(current, 0.9f, m_targetCollider);
      }
      dir += obstacleAvoid * 1.5f;
      if (dir.Magnitude() > 0.01f)
      {
          dir = dir.Normalize();
      }

      MathEngine::Vector2 nextPos = current + dir * m_moveSpeed * deltaTime;

      int nextTileX = (int)round(nextPos.x);
      int nextTileY = (int)round(nextPos.y);

      bool blockedByTile = (m_collisionManager != nullptr && m_collisionManager->IsBlocked(nextTileX, nextTileY));

      bool blockedByPhysics = false;
      //if (!blockedByTile && m_physicsWorld != nullptr)
      //{
      //    PhysicsEngine::CircleCollider testCollider(0.f, 0.f, 0.4f);
      //    testCollider.center = nextPos;
      //    blockedByPhysics = m_physicsWorld->IsColliderBlocked(testCollider, m_targetPhysicsObject,true);
      //    m_physicsWorld->PushDynamicObjects(testCollider, current, 0.4f, m_targetPhysicsObject);
      //}
      if (!blockedByTile && m_collider != nullptr)
      {
          blockedByPhysics = m_collider->IsPositionBlocked(nextPos, 0.4f, m_targetCollider, true);
          m_collider->PushNearbyDynamics(nextPos, 0.4f, current, m_targetCollider);
      }

      if (!blockedByTile && !blockedByPhysics)
      {
          m_transform->SetPosition(nextPos);
          return;
      }
      const float tryAngles[] = { 30.0f, -30.0f, 60.0f, -60.0f, 90.0f, -90.0f };
      for (float angleDeg : tryAngles)
      {
          float rad = angleDeg * 3.14159265f / 180.0f;
          float cosA = cosf(rad);
          float sinA = sinf(rad);

          MathEngine::Vector2 altDir;
          altDir.x = dir.x * cosA - dir.y * sinA;
          altDir.y = dir.x * sinA + dir.y * cosA;

          MathEngine::Vector2 altPos = current + altDir * m_moveSpeed * deltaTime;

          int altTileX = (int)round(altPos.x);
          int altTileY = (int)round(altPos.y);
          bool altBlockedByTile = (m_collisionManager != nullptr && m_collisionManager->IsBlocked(altTileX, altTileY));

          bool altBlockedByPhysics = false;
          //if (!altBlockedByTile && m_physicsWorld != nullptr)
          //{
          //    PhysicsEngine::CircleCollider testCollider2(0.f, 0.f, 0.4f);
          //    testCollider2.center = altPos;
          //    altBlockedByPhysics = m_physicsWorld->IsColliderBlocked(testCollider2,m_targetPhysicsObject,true);
          //    m_physicsWorld->PushDynamicObjects(testCollider2, current, 0.4f, m_targetPhysicsObject);
          //}
          if (!altBlockedByTile && m_collider != nullptr)
          {
              altBlockedByPhysics = m_collider->IsPositionBlocked(altPos, 0.4f, m_targetCollider, true);
              m_collider->PushNearbyDynamics(altPos, 0.4f, current, m_targetCollider);
          }

          if (!altBlockedByTile && !altBlockedByPhysics)
          {
              m_transform->SetPosition(altPos);
              return;
          }
      }
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

    if (m_animator != nullptr)
    {
        m_animator->SetCellSize(cellWidth, cellHeight);
        m_animator->SetAnimation(animColumn, 0, 0.15f);
    }
}
