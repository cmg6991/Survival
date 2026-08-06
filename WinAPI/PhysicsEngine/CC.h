///
/// CC 충돌 처리 알고리즘
/// 두 원의 중심점과 반지름을 이용하여 충돌 여부를 판단
///

#pragma once

#include "ICollisionDetection.h"
#include "Vector2.h"

class CC
	: public ICollisionDetection
{
public:
	CC();
	~CC();

	virtual void CheckCollision() override;
	virtual void Render(DirectY& directY) const override;
	

	void SetCollisionTarget(CC* val) { m_circleTarget = val; }

	bool GetCollide() const { return m_isCollide; }
	void SetCollide(bool val) { m_isCollide = val; }


	Vector2 GetCenterPos() const { return m_centerPos; }
	void SetCenterPos(Vector2 val) { m_centerPos = val; }

	float GetRadius() const { return m_radius; }
	void SetRadius(float val) { m_radius = val; }

private:
	CC* m_circleTarget;
	bool m_isCollide;

	Vector2 m_centerPos;
	float m_radius;

	bool CalculateCollision(const CC& other) const;
};

