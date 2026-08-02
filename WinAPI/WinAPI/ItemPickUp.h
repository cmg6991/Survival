#pragma once
#include "ElementBase.h"
#include "pch.h"

class Transform;

class ItemPickUp : public ElementBase
{
public:
	ItemPickUp(string itemId, int count);
	~ItemPickUp();
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

	bool IsCollected() const { return m_collected; }
	const string& GetItemId() const { return m_itemId; }
	int GetCount() const { return m_count; }

private:
	string m_itemId;
	int m_count;
	Transform* m_transform;
	bool m_collected = false;
};

