#include "Player.h"
#include "InputManager.h"
#include "TileMap.h"
#include "CameraManager.h"
#include "ResourceManager.h"
#include "CollisionManager.h"
#include "DrawUtil.h"
#include "Transform.h"
#include "GameObject.h"
#include "Vector2.h"
#include <iostream>
#include "TileManager.h"
#include "Animator.h"
#include "SpriteRenderer.h"
using namespace std;

Player::Player() 
    : m_transform(nullptr), m_targetPos(0), 
    m_moveSpeed(10.f), m_tileMap(nullptr), m_collisionManager(nullptr),m_animator(nullptr),m_sprite(nullptr)
{
}

Player::~Player()
{
}

void Player::Init()
{
    m_transform =static_cast<Transform*>(m_gameObject->GetElement(ElementType::Transform));
    m_targetPos = m_transform->GetPostion();

    m_animator = static_cast<Animator*>(m_gameObject->GetElement(ElementType::Animator));
    m_sprite = static_cast<SpriteRenderer*>(m_gameObject->GetElement(ElementType::SpriteRenderer));

}

void Player::FixedUpdate()
{
}

void Player::Update(float deltaTime)
{
    MathEngine::Vector2 current = m_transform->GetPostion();

    MathEngine::Vector2 freeDir = { 0,0 };

    if (InputManager::GetInstance().IsGetKey('A'))
        freeDir.x--;

    if (InputManager::GetInstance().IsGetKey('D'))
        freeDir.x++;

    if (InputManager::GetInstance().IsGetKey('W'))
        freeDir.y--;

    if (InputManager::GetInstance().IsGetKey('S'))
        freeDir.y++;

    bool isKeyMoving = (freeDir.Magnitude() > 0.001f);

    if (isKeyMoving)
    {
        m_isAutoMoving = false;
        m_targetPos = current; // 목표점 개념을 현재 위치로 리셋해서 자동이동 잔재 제거
    }

    // 마우스 좌클릭 이동
    if (InputManager::GetInstance().IsGetKeyDown(VK_LBUTTON))
    {
        MathEngine::Vector2 mousePos = InputManager::GetInstance().GetMousePosition();

        MathEngine::Vector2 world =
        {
            mousePos.x + CameraManager::GetInstance().GetX(),
            mousePos.y + CameraManager::GetInstance().GetY()
        };

        MathEngine::Vector2 tile = TileManager::GetInstance().ScreenToTile(world);

        int tileX = (int)round(tile.x);
        int tileY = (int)round(tile.y);

        if (!m_collisionManager->IsBlocked((int)tileX,(int)tileY))
        {
            m_targetPos.x = tileX;
            m_targetPos.y = tileY;
            m_isAutoMoving = true;
        }
    }

    MathEngine::Vector2 dir = { 0, 0 };
    MathEngine::Vector2 nextPos = current;

    if (m_isAutoMoving)
    {
        // 기존 방식: 목표점까지 부드럽게 자동 이동
        dir = m_targetPos - current;
        float dist = dir.Magnitude();

        if (dist > 0.001f)
        {
            dir = dir.Normalize();
            float moveAmount = m_moveSpeed * deltaTime;

            if (moveAmount >= dist)
                nextPos = m_targetPos;
            else
                nextPos = current + dir * moveAmount;
        }
        else
        {
            m_isAutoMoving = false; // 도착하면 자동이동 종료
        }
    }
    else if (isKeyMoving)
    {
        // 새 방식: 키 입력 방향으로 즉시 이동, 떼면 즉시 멈춤
        dir = freeDir.Normalize();
        nextPos = current + dir * m_moveSpeed * deltaTime;
    }

    //MathEngine::Vector2 dir =m_targetPos - current;

    int dx = 0;
    int dy = 0;

    if (dir.x > 0.1f)
        dx = 1;
    else if (dir.x < -0.1f)
        dx = -1;

    if (dir.y > 0.1f)
        dy = 1;
    else if (dir.y < -0.1f)
        dy = -1;

    if (dx == 0 && dy == 1)
    {
        m_animator->SetAnimation(6, 0, 0.1f);
        m_sprite->SetFlip(false);
    }

    else if (dx == -1 && dy == 1)
    {
        m_animator->SetAnimation(6, 1, 0.1f);
        m_sprite->SetFlip(false);
    }

    else if (dx == -1 && dy == 0)
    {
        m_animator->SetAnimation(6, 2, 0.1f);
        m_sprite->SetFlip(false);
    }

    else if (dx == -1 && dy == -1)
    {
        m_animator->SetAnimation(6, 3, 0.1f);
        m_sprite->SetFlip(false);
    }

    else if (dx == 0 && dy == -1)
    {
        m_animator->SetAnimation(6, 4, 0.1f);
        m_sprite->SetFlip(false);
    }

    else if (dx == 1 && dy == -1)
    {
        m_animator->SetAnimation(6, 3, 0.1f);
        m_sprite->SetFlip(true);
    }

    else if (dx == 1 && dy == 0)
    {
        m_animator->SetAnimation(6, 2, 0.1f);
        m_sprite->SetFlip(true);
    }

    else if (dx == 1 && dy == 1)
    {
        m_animator->SetAnimation(6, 1, 0.1f);
        m_sprite->SetFlip(true);
    }

    //float dist = dir.Magnitude();

    //if (dist > 0.001f)
    //{
    //    dir = dir.Normalize();

    //    float moveAmount = m_moveSpeed * deltaTime;

    //    MathEngine::Vector2 nextPos;

    //    if (moveAmount >= dist)
    //    {
    //        nextPos = m_targetPos;
    //    }
    //    else
    //    {
    //        nextPos = current + dir * moveAmount;
    //    }

    //    // 이동하려는 위치의 타일 계산
    //    int tileX = (int)round(nextPos.x);
    //    int tileY = (int)round(nextPos.y);

    //    // 벽이 아니면 이동
    //    if (!m_collisionManager->IsBlocked(tileX, tileY))
    //    {
    //        current = nextPos;
    //        m_transform->SetPosition(current);
    //    }
    //    else
    //    {
    //        // 벽에 닿았으면 이동 중지
    //        m_targetPos = current;
    //    }
    //}
        // 실제 이동 (충돌 체크 후 적용)
    if (isKeyMoving || m_isAutoMoving)
    {
        int tileX = (int)round(nextPos.x);
        int tileY = (int)round(nextPos.y);

        if (!m_collisionManager->IsBlocked(tileX, tileY))
        {
            current = nextPos;
            m_transform->SetPosition(current);
        }
        else if (m_isAutoMoving)
        {
            m_targetPos = current; // 자동이동 중 벽 만나면 멈춤
            m_isAutoMoving = false;
        }
    }
}

void Player::LateUpdate()
{
}

void Player::PreRender()
{
}

void Player::Render(ID2D1DeviceContext* context)
{
    MathEngine::Vector2 screen = TileManager::GetInstance().TileToScreen( m_transform->GetPostion());

    int screenX = (int)(screen.x - CameraManager::GetInstance().GetX());
    int screenY = (int)(screen.y - CameraManager::GetInstance().GetY());
}

void Player::PostRender(ID2D1DeviceContext* context)
{
}

void Player::Release()
{
}

ElementType Player::GetElementType() const
{
    return ElementType::Player;
}

GameObject* Player::GetGameObject() const
{
    return m_gameObject;
}

void Player::SetGameObject(GameObject* gameObject)
{
    m_gameObject = gameObject;
}
