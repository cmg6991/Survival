///
/// AABB 충돌 처리 알고리즘
/// 객체들의 최소, 최대 x, y 좌표를 이용하여 충돌 여부를 판단
/// 

#pragma once

#include "ICollisionDetection.h"
#include "Vector2.h"

class AABB
	: public ICollisionDetection
{
public:
	AABB();
	~AABB();

	virtual void CheckCollision() override;
	virtual void Render(DirectY& directY) const override;


	void SetAabbTarget(AABB* val) { m_aabbTarget = val; }

	bool GetCollide() const { return m_isCollide; }
	void SetCollide(bool val) { m_isCollide = val; }


	Vector2 GetLeftTop() const { return m_leftTop; }
	void SetLeftTop(Vector2 val) { m_leftTop = val; }

	Vector2 GetRightBottom() const { return m_rightBottom; }
	void SetRightBottom(Vector2 val) { m_rightBottom = val; }

private:
	AABB* m_aabbTarget;
	bool m_isCollide;

	Vector2 m_leftTop;
	Vector2 m_rightBottom;

	bool CalculateCollision(const AABB& other) const;
};

