#include "Chicken.h"
#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "ResourceNode.h"
#include "pch.h"
#include "TileMap.h"

Chicken::Chicken()
{
}

Chicken::~Chicken()
{
}

void Chicken::Init()
{
    m_transform = static_cast<Transform*>(m_gameObject->GetElement(ElementType::Transform));
    m_animator = static_cast<Animator*>(m_gameObject->GetElement(ElementType::Animator));
    m_sprite =static_cast<SpriteRenderer*>(m_gameObject->GetElement(ElementType::SpriteRenderer));

    if (m_animator != nullptr)
    {
        m_animator->SetCellSize(32, 32);
        m_animator->SetAnimation(4, 0, 0.25f);
    }

    m_eggTimer = m_eggInterval;
}

void Chicken::FixedUpdate()
{
}

void Chicken::Update(float deltaTime)
{
    UpdateMovement(deltaTime);

    if (m_resourceNode == nullptr)
        return;

    if (!m_canProduceEgg)
        return;

    if (m_transform == nullptr)
        return;

    m_eggTimer -= deltaTime;

    if (m_eggTimer > 0.0f)
        return;

    m_eggTimer = m_eggInterval;

    // 현재 닭 위치
    MathEngine::Vector2 pos =
        m_transform->GetPostion();

    // ResourceNode에서 알 정보 가져오기
    string eggItemId = m_resourceNode->GetItemId();

    if (eggItemId.empty())
        return;

    int minCount = m_resourceNode->GetMinCount();
    int maxCount = m_resourceNode->GetMaxCount();

    int count = minCount;

    if (maxCount > minCount)
    {
        static mt19937 rng(random_device{}());

        uniform_int_distribution<int> dist(
            minCount,
            maxCount);

        count = dist(rng);
    }

    if (m_eggDropCallback)
    {
        m_eggDropCallback(
            pos,
            eggItemId,
            count);
    }
}

void Chicken::LateUpdate()
{
}

void Chicken::PreRender()
{
}

void Chicken::Render(ID2D1DeviceContext* context)
{
}

void Chicken::PostRender(ID2D1DeviceContext* context)
{
}

void Chicken::Release()
{
}

ElementType Chicken::GetElementType() const
{
    return ElementType::Chicken;
}

GameObject* Chicken::GetGameObject() const
{
    return m_gameObject;
}

void Chicken::SetGameObject(GameObject* gameObject)
{
    m_gameObject = gameObject;
}

void Chicken::SetEggDropCallback(
    std::function<void(
        const MathEngine::Vector2&,
        const string&,
        int)> callback)
{
    m_eggDropCallback = callback;
}

void Chicken::UpdateMovement(float deltaTime)
{
    if (m_transform == nullptr)
        return;


    static mt19937 rng(random_device{}());

    m_moveTimer -= deltaTime;

    // ==========================================
    // 이동 / 정지 상태 변경
    // ==========================================
    if (m_moveTimer <= 0.0f)
    {
        if (m_isMoving)
        {
            // 이동 -> 정지
            m_isMoving = false;
            m_moveDir = { 0.0f, 0.0f };

            uniform_real_distribution<float> idleDist(1.0f, 3.0f);
            m_moveTimer = idleDist(rng);

            if (m_animator != nullptr)
                m_animator->Pause();
        }
        else
        {
            // 정지 -> 이동
            m_isMoving = true;

            uniform_real_distribution<float> angleDist(
                0.0f,
                6.2831853f
            );

            float angle = angleDist(rng);

            m_moveDir =
            {
                cosf(angle),
                sinf(angle)
            };

            uniform_real_distribution<float> moveDist(
                1.0f,
                2.5f
            );

            m_moveTimer = moveDist(rng);

            float screenDirX =
                m_moveDir.x - m_moveDir.y;

            if (m_sprite != nullptr)
            {
                m_sprite->SetFlip(
                    screenDirX > 0.0f
                );
            }

            if (m_animator != nullptr)
                m_animator->Resume();
        }
    }

    if (!m_isMoving)
        return;

    // ==========================================
    // 현재 위치
    // ==========================================
    MathEngine::Vector2 current =
        m_transform->GetPostion();

    // ==========================================
    // 다음 위치
    // ==========================================
    MathEngine::Vector2 nextPos =
        current +
        m_moveDir * m_moveSpeed * deltaTime;

    // ==========================================
    // 다음 타일
    // ★ 몬스터와 완전히 동일한 방식
    // ==========================================
    int nextTileX =
        (int)round(nextPos.x);

    int nextTileY =
        (int)round(nextPos.y);

    // ==========================================
    // 물 검사
    // ==========================================
    bool blockedByWater =
        (m_tileMap != nullptr &&
            m_tileMap->IsWater(nextTileX, nextTileY));

    // ==========================================
    // 물이면 이동 금지
    // ==========================================
    if (blockedByWater)
    {
        uniform_real_distribution<float> angleDist(
            0.0f,
            6.2831853f
        );

        bool foundDirection = false;

        // ======================================
        // 새로운 방향 여러 번 시도
        // ======================================
        for (int i = 0; i < 30; ++i)
        {
            float angle = angleDist(rng);

            MathEngine::Vector2 newDir =
            {
                cosf(angle),
                sinf(angle)
            };

            if (newDir.Magnitude() > 0.01f)
                newDir = newDir.Normalize();

            // ==================================
            // 새로운 위치
            // ==================================
            MathEngine::Vector2 testPos =
                current +
                newDir * m_moveSpeed * deltaTime;

            int testTileX =
                (int)round(testPos.x);

            int testTileY =
                (int)round(testPos.y);

            // ==================================
            // 물 검사
            // ==================================
            bool testBlockedByWater =
                (m_tileMap != nullptr &&
                    m_tileMap->IsWater(
                        testTileX,
                        testTileY
                    ));

            // 물이면 이 방향 버림
            if (testBlockedByWater)
                continue;

            // ==================================
            // 안전한 방향 발견
            // ==================================
            m_moveDir = newDir;

            float screenDirX =
                m_moveDir.x - m_moveDir.y;

            if (m_sprite != nullptr)
            {
                m_sprite->SetFlip(
                    screenDirX > 0.0f
                );
            }

            foundDirection = true;
            break;
        }

        // ======================================
        // 안전한 방향을 못 찾음
        // ======================================
        if (!foundDirection)
        {
            m_isMoving = false;
            m_moveDir = { 0.0f, 0.0f };

            m_moveTimer = 1.0f;

            if (m_animator != nullptr)
                m_animator->Pause();
        }
        return;
    }

    // ==========================================
    // 최종 이동
    // ==========================================
    m_transform->SetPosition(nextPos);
}
