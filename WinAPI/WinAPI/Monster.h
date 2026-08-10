#pragma once

#include "ElementBase.h"

class Transform;
class Animator;

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

private:
    Transform* m_transform = nullptr;
    Animator* m_animator = nullptr;
    int m_health;
};

