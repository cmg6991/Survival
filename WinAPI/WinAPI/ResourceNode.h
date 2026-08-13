#pragma once
#include "pch.h"
#include "ElementBase.h"

class ResourceNode :public ElementBase
{
public:
	ResourceNode(const string& itemId, int minCount, int maxCount);
	~ResourceNode();

	virtual void Init() override {}

	virtual void FixedUpdate() override {}
	virtual void Update(float deltaTime) override{}
	virtual void LateUpdate() override{}

	virtual void PreRender() override{}
	virtual void Render(ID2D1DeviceContext* context) override{}
	virtual void PostRender(ID2D1DeviceContext* context) override{}

	virtual void Release() override{}

	virtual ElementType GetElementType() const override { return ElementType::ResourceNode; }

	virtual GameObject* GetGameObject() const override { return m_gameObject; }
	virtual void SetGameObject(GameObject* gameObject) override { m_gameObject = gameObject; }

	const string& GetItemId() const { return m_itemId;}

	int GetMinCount() const { return m_minCount;}
	int GetMaxCount() const { return m_maxCount; }

private:
	string m_itemId;
	int m_minCount;
	int m_maxCount;
};

