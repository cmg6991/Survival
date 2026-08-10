#pragma once

#include "ElementBase.h"

class AttackHitBox : public ElementBase
{
public:
    AttackHitBox(int damage, float lifeTime = 0.1f);
    ~AttackHitBox();

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

    bool IsExpired() const { return m_timer <= 0.0f; }
    int GetDamage() const { return m_damage; }

private:
    int m_damage;
    float m_timer;
};

