#pragma once

#include "ElementBase.h"
#include "../MathEngine/Vector2.h"

class Transform;
class Animator;
class CollisionManager;
class FlowFieldManager;
class ColliderComponent;
class TileMap;
class SpriteRenderer;
//namespace PhysicsEngine
//{
//    class PhysicsWorld;
//    struct Object;
//}

class Monster : public ElementBase
{
public:
    Monster(int maxHealth);
    ~Monster();

    void Init() override;
    void FixedUpdate() override;
    void Update(float deltaTime) override;
    void LateUpdate() override;
    void PreRender() override;
    void Render(ID2D1DeviceContext* context) override;
    void PostRender(ID2D1DeviceContext* context) override;
    void Release() override;

    ElementType GetElementType() const override;
    GameObject* GetGameObject() const override;
    void SetGameObject(GameObject* gameObject) override;

    void TakeDamage(int amount);
    bool IsDead() const { return m_health <= 0; }
    Transform* GetTransform() const { return m_transform; }

    void SetTarget(Transform* target) { m_target = target; }
    void SetCollisionManager(CollisionManager* cm) { m_collisionManager = cm; }
    //void SetPhysicsWorld(PhysicsEngine::PhysicsWorld* world) { m_physicsWorld = world; }
    void SetFlowField(FlowFieldManager* flowField) { m_flowField = flowField; }
    void SetStats(float moveSpeed, int contactDamage) { m_moveSpeed = moveSpeed; m_contactDamage = contactDamage; }
    void SetTileMap(TileMap* tileMap) { m_tileMap = tileMap; }

    int GetContactDamage() const { return m_contactDamage; }
    bool CanDealDamage() const { return m_damageCooldownTimer <= 0.0f; }
    void ResetDamageCooldown() { m_damageCooldownTimer = m_damageCooldownInterval; }
    void SetSeparation(const MathEngine::Vector2& separation) {m_separation = separation;}

    void SetTargetCollider(ColliderComponent* targetCollider) { m_targetCollider = targetCollider; }
   //void SetTargetPhysicsObject(PhysicsEngine::Object* obj) { m_targetPhysicsObject = obj; }

    void Reset(int maxHealth);

    void SetMonsterId(const string& id) { m_monsterId = id; }
    const string& GetMonsterId() const { return m_monsterId; }
    void Reset(int maxHealth, int cellWidth, int cellHeight, int animColumn);

    bool IsReadyToRemove() const { return m_isDying && m_deathTimer <= 0.0f; }

    void ForceStartFadeOut();
private:
    Transform* m_transform = nullptr;
    Animator* m_animator = nullptr;
    Transform* m_target = nullptr;
    CollisionManager* m_collisionManager = nullptr;
    FlowFieldManager* m_flowField = nullptr;
    ColliderComponent* m_collider = nullptr;
    ColliderComponent* m_targetCollider = nullptr;
    TileMap* m_tileMap = nullptr;
    SpriteRenderer* m_sprite = nullptr;

    int m_health;
    float m_moveSpeed = 1.5f;
    int m_contactDamage = 5;

    float m_damageCooldownTimer = 0.0f;
    const float m_damageCooldownInterval = 1.0f;

    MathEngine::Vector2 m_separation = { 0.0f, 0.0f };

    MathEngine::Vector2 m_avoidDirection = { 0.0f, 0.0f };
    float m_avoidTimer = 0.0f;

    string m_monsterId;

    bool m_isDying = false;
    float m_deathTimer = 0.0f;
    const float m_deathDuration = 0.6f;
};

