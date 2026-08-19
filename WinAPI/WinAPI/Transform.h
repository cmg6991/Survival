#pragma once
#include "ElementBase.h"
#include "../MathEngine/Vector2.h"

class Transform : public ElementBase
{
public:
	Transform();
	~Transform();

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

	//int GetTileX() const { return (int)m_position.x; }
	//int GetTileY() const { return (int)m_position.y; }

	/*void SetTilePosition(int x, int y)
	{
		m_position.x =(float)x;
		m_position.y = (float)y;
	}*/

	MathEngine::Vector2 GetPostion() const { return m_position; }
	void SetPosition(const MathEngine::Vector2& pos) { m_position = pos; }

	void SetFloatX(const float x) { m_position.x = x; }
	void SetFloatY(const float y) { m_position.y = y; }

private:
	//int m_tileX;
	//int m_tileY;

	MathEngine::Vector2 m_position = { 0,0 };
};

