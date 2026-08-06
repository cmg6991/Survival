#include "AABB.h"
#include "DirectY.h"

AABB::AABB()
	: m_leftTop{ 0.0f, 0.0f }
	, m_rightBottom{ 0.0f, 0.0f }
	, m_isCollide(false)
	, m_aabbTarget(nullptr)
{

}

AABB::~AABB()
{

}

void AABB::CheckCollision()
{
	if (m_aabbTarget != nullptr)
	{
		SetCollide(CalculateCollision(*m_aabbTarget));
	}
}

void AABB::Render(DirectY& directY) const
{
	if (GetCollide())
	{
		directY.SetBrushColor(D2D1::ColorF::Red);
	}
	else
	{
		directY.SetBrushColor(D2D1::ColorF::Green);
	}
	directY.DrawRectangle(D2D1_RECT_F{ GetLeftTop().m_x, GetLeftTop().m_y, GetRightBottom().m_x , GetRightBottom().m_y }, 2.8f);
}

bool AABB::CalculateCollision(const AABB& other) const
{
	if (
		GetLeftTop().m_x < other.GetRightBottom().m_x &&
		GetRightBottom().m_x > other.GetLeftTop().m_x &&
		GetLeftTop().m_y < other.GetRightBottom().m_y &&
		GetRightBottom().m_y > other.GetLeftTop().m_y
		)
	{
		return true;
	}

	return false;
}
