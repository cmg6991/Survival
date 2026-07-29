#pragma once
#include "ElementBase.h"

class Transform;

class Wall : public ElementBase
{
public:
    Wall();
    ~Wall();

    virtual void Init() override;

    virtual void Update(float deltaTime) override;
    virtual void Release() override;

    virtual ElementType GetElementType() const override;

    virtual GameObject* GetGameObject() const override;
    virtual void SetGameObject(GameObject* gameObject) override;

private:
    Transform* m_transform;
    GameObject* m_gameObject;
};

