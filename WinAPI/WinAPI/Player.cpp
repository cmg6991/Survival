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
#include "Weapon.h"

using namespace std;

Player::Player() 
    : m_transform(nullptr), m_targetPos(0), 
    m_moveSpeed(10.f), m_tileMap(nullptr), m_collisionManager(nullptr),m_animator(nullptr),m_sprite(nullptr),m_weapon(nullptr)
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
    UpdateSpriteState();
}

void Player::FixedUpdate()
{
}

void Player::Update(float deltaTime)
{
    if (m_isAttacking)
    {
        m_attackTimer -= deltaTime;
        if (m_attackTimer <= 0.0f)
        {
            m_isAttacking = false;
            UpdateSpriteState(); // 공격 끝 -> Walk/Idle로 복귀
        }
    }

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

    if (InputManager::GetInstance().IsGetKeyDown(VK_RBUTTON))
    {
        Attack();
    }

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

    bool isMoving = (dx != 0 || dy != 0);

    if (isMoving != m_isMoving)
    {
        m_isMoving = isMoving;
        UpdateSpriteState();
    }

    bool isRangedEquipped = (m_weapon != nullptr && m_weapon->GetWeaponType() == WeaponType::Ranged);

    if (isMoving)
    {
        // 움직이는 동안은 항상 이동 방향을 바라봄 (총이든 검이든 동일)
        ApplyFacing(dir);
    }
    else if (isRangedEquipped)
    {
        // 멈춰있을 때만 총이면 마우스를 조준
        MathEngine::Vector2 mouseScreen = InputManager::GetInstance().GetMousePosition();
        MathEngine::Vector2 mouseWorld =
        {
            mouseScreen.x + CameraManager::GetInstance().GetX(),
            mouseScreen.y + CameraManager::GetInstance().GetY()
        };
        MathEngine::Vector2 mouseTile = TileManager::GetInstance().ScreenToTile(mouseWorld);
        ApplyFacing(mouseTile - current);
    }

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

    if (m_weapon != nullptr)
    {
        float offsetX = m_sprite->GetFlip() ? 0.2f : -0.2f;
        m_weapon->GetTransform()->SetPosition(
            {
                m_transform->GetPostion().x + offsetX,
                m_transform->GetPostion().y
            });
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

//void Player::SetEquipWeapon(const string& weaponId)
//{
//    m_equippedWeapon = weaponId;
//}

//void Player::EquipWeapon(string weaponID)
//{
//    if (m_weapon == nullptr)
//        return;
//
//    m_equippedWeapon = weaponID;
//
//    m_weapon->SetEquipped(true);
//
//    OutputDebugStringA("무기 장착 완료\n");
//}

void Player::Attack()
{
    if (m_weapon)
        m_weapon->Attack();
    m_isAttacking = true;
    m_attackTimer = m_attackDuration;
    UpdateSpriteState();
}

void Player::SetArmedVisaul(const string& spriteKey)
{
    m_currentArmedSprite = spriteKey;
    UpdateSpriteState();

}

void Player::ClearArmedVisual()
{
    m_currentArmedSprite = "";
    UpdateSpriteState();
}

void Player::SetShield(const string& spriteKey, int defense)
{
    m_shieldSpriteKey = spriteKey;
    m_shieldDefense = defense;
    UpdateSpriteState();
}

void Player::ClearShield()
{
    m_shieldSpriteKey = "";
    m_shieldDefense = 0;
    UpdateSpriteState();
}

void Player::UpdateSpriteState()
{
    if (m_sprite == nullptr) return;

    string baseKey;
    if (!m_currentArmedSprite.empty())
        baseKey = m_currentArmedSprite;
    else if (!m_shieldSpriteKey.empty())
        baseKey = m_shieldSpriteKey;
    else
        baseKey = "Player";

    string suffix;
    if (m_isAttacking)
        suffix = "_Attack";
    else
        suffix = m_isMoving ? "_Walk" : "_Idle";

    string finalKey = baseKey + suffix;

    if (finalKey == m_lastAppliedSprite) return;
    m_lastAppliedSprite = finalKey;

    m_sprite->ChangeImage(finalKey);
}

void Player::ApplyFacing(const MathEngine::Vector2& faceDir)
{
    if (faceDir.Magnitude() < 0.001f)
        return;

    const float PI = 3.14159265f;
    float angle = atan2(faceDir.y, faceDir.x); // -PI ~ PI

    int octant = (int)std::round(angle / (PI / 4.0f));
    octant = ((octant % 8) + 8) % 8; // 0~7로 정규화

    int row = -1;
    bool flip = false;

    switch (octant)
    {
    case 0: row = 2; flip = true;  break; // 오른쪽
    case 1: row = 1; flip = true;  break; // 오른쪽-아래
    case 2: row = 0; flip = false; break; // 아래
    case 3: row = 1; flip = false; break; // 왼쪽-아래
    case 4: row = 2; flip = false; break; // 왼쪽
    case 5: row = 3; flip = false; break; // 왼쪽-위
    case 6: row = 4; flip = false; break; // 위
    case 7: row = 3; flip = true;  break; // 오른쪽-위
    }

    m_animator->SetAnimation(6, row, 0.1f);
    m_sprite->SetFlip(flip);
}
