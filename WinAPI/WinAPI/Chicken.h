#pragma once
#include "ElementBase.h"
#include "../MathEngine/Vector2.h"
#include "pch.h"

class Transform;
class SpriteRenderer;
class Animator;
class ResourceNode;
class TileMap;

class Chicken : public ElementBase
{
public:
    Chicken();
    ~Chicken();

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
    void SetResourceNode(ResourceNode* resourceNode) { m_resourceNode = resourceNode; }
    // 알을 바닥에 떨어뜨리는 함수
    void SetEggDropCallback(
        std::function<void(const MathEngine::Vector2&, const string&, int)> callback);
    void SetTileMap(TileMap* tileMap) { m_tileMap = tileMap; }

private:
    Transform* m_transform = nullptr;
    SpriteRenderer* m_sprite = nullptr;
    Animator* m_animator = nullptr;
    ResourceNode* m_resourceNode = nullptr;
    TileMap* m_tileMap = nullptr;

    float m_eggTimer = 0.0f;

    // 알 생산 간격
    float m_eggInterval = 30.0f;

    // 알 생산 여부
    bool m_canProduceEgg = true;

    function<void(const MathEngine::Vector2&, const string&, int)> m_eggDropCallback;

    // ── 이동 관련 ──
    void UpdateMovement(float deltaTime);

    MathEngine::Vector2 m_moveDir = { 0.0f, 0.0f };
    float m_moveSpeed = 1.5f;      // 초당 이동 거리 (타일 좌표계 기준, 필요시 조절)
    float m_moveTimer = 0.0f;      // 현재 상태(이동/대기) 남은 시간
    bool m_isMoving = false;
};