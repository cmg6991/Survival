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

    if (m_moveTimer <= 0.0f)
    {
        if (m_isMoving)
        {
            m_isMoving = false;
            m_moveDir = { 0.0f, 0.0f };

            uniform_real_distribution<float> idleDist(1.0f, 3.0f);
            m_moveTimer = idleDist(rng);

            if (m_animator != nullptr)
                m_animator->Pause();
        }
        else
        {
            m_isMoving = true;

            uniform_real_distribution<float> angleDist(
                0.0f,
                6.2831853f);

            float angle = angleDist(rng);

            m_moveDir =
            {
                cosf(angle),
                sinf(angle)
            };

            uniform_real_distribution<float> moveDist(
                1.0f,
                2.5f);

            m_moveTimer = moveDist(rng);

            float screenDirX =
                m_moveDir.x - m_moveDir.y;

            if (m_sprite != nullptr)
                m_sprite->SetFlip(screenDirX > 0.0f);

            if (m_animator != nullptr)
                m_animator->Resume();
        }
    }

    if (!m_isMoving)
        return;

    MathEngine::Vector2 pos =
        m_transform->GetPostion();

    MathEngine::Vector2 nextPos = pos;

    nextPos.x +=
        m_moveDir.x *
        m_moveSpeed *
        deltaTime;

    nextPos.y +=
        m_moveDir.y *
        m_moveSpeed *
        deltaTime;

    // ==========================================
    // 물 검사
    // ==========================================
    if (m_tileMap != nullptr)
    {
        int currentX =
            static_cast<int>(floorf(pos.x));

        int currentY =
            static_cast<int>(floorf(pos.y));

        int nextX =
            static_cast<int>(floorf(nextPos.x));

        int nextY =
            static_cast<int>(floorf(nextPos.y));

        TileType nextTile =
            m_tileMap->GetTile(nextX, nextY);

        // ==========================================
        // 다음 위치가 물이면
        // ==========================================
        if (nextTile == TileType::WATER)
        {
            // 현재 이동 방향은 폐기
            m_moveDir = { 0.0f, 0.0f };

            // 새로운 방향을 여러 번 시도
            uniform_real_distribution<float> angleDist(
                0.0f,
                6.2831853f);

            bool foundDirection = false;

            for (int i = 0; i < 20; ++i)
            {
                float angle = angleDist(rng);

                MathEngine::Vector2 newDir =
                {
                    cosf(angle),
                    sinf(angle)
                };

                MathEngine::Vector2 testPos = pos;

                testPos.x +=
                    newDir.x *
                    m_moveSpeed *
                    deltaTime;

                testPos.y +=
                    newDir.y *
                    m_moveSpeed *
                    deltaTime;

                int testX =
                    static_cast<int>(floorf(testPos.x));

                int testY =
                    static_cast<int>(floorf(testPos.y));

                TileType testTile =
                    m_tileMap->GetTile(testX, testY);

                // 물이 아니면 이 방향 사용
                if (testTile != TileType::WATER)
                {
                    m_moveDir = newDir;

                    float screenDirX =
                        m_moveDir.x -
                        m_moveDir.y;

                    if (m_sprite != nullptr)
                        m_sprite->SetFlip(
                            screenDirX > 0.0f);

                    foundDirection = true;
                    break;
                }
            }

            // 새로운 방향을 찾았으면
            // 이번 프레임에는 이동하지 않고
            // 다음 프레임부터 이동
            if (foundDirection)
            {
                return;
            }

            // 아무 방향도 못 찾았으면 정지
            m_isMoving = false;
            m_moveTimer = 1.0f;

            if (m_animator != nullptr)
                m_animator->Pause();

            return;
        }
    }

    // ==========================================
    // 물이 아니면 정상 이동
    // ==========================================
    m_transform->SetPosition(nextPos);
}
