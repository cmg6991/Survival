#include "CC.h"
#include "DirectY.h"

CC::CC()
	: m_centerPos{0.0f, 0.0f}
	, m_radius(0.0f)
	, m_isCollide(false)
	, m_circleTarget(nullptr)
{

}

CC::~CC()
{

}

void CC::CheckCollision()
{
	if (m_circleTarget != nullptr)
	{
		m_isCollide =  CalculateCollision(*m_circleTarget);
	}
}

void CC::Render(DirectY& directY) const
{
	if (m_isCollide)
	{
		directY.SetBrushColor(D2D1::ColorF::Red);
	}
	else
	{
		directY.SetBrushColor(D2D1::ColorF::Green);
	}

	directY.DrawCircle(D2D1_POINT_2F{ m_centerPos.m_x, m_centerPos.m_y }, m_radius, 2.8f);
}

bool CC::CalculateCollision(const CC& other) const
{ 
	float distance = m_centerPos.Distance(other.GetCenterPos());

	if (distance <= m_radius + other.GetRadius())
	{
		return true;
	}

	return false;
}
