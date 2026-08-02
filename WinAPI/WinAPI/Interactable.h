#pragma once
#include "ElementBase.h"

enum class InteractType
{
	CampFire,
	WorkTable
};

class Interactable : public ElementBase
{
public:
	Interactable(InteractType type);
	~Interactable() {}
	virtual void Init() override;

	virtual void FixedUpdate() override;
	virtual void Update(float deltaTime) override;
	virtual void LateUpdate() override;

	virtual void PreRender() override;
	virtual void Render(ID2D1DeviceContext* context) override;
	virtual void PostRender(ID2D1DeviceContext* context) override;

	virtual void Release() override;

	virtual ElementType GetElementType() const override;

	virtual GameObject* GetGameObject() const override;
	virtual void SetGameObject(GameObject* gameObject) override;

	InteractType GetInteractType() const { return m_interactType; }

private:
	InteractType m_interactType;
};

