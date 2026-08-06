#include "OBB.h"
#include "DirectY.h"
#include "MakeMatrix.h"

OBB::OBB()
	: m_obbTarget(nullptr)
	, m_isCollide(false)
	, m_centerPos{0.0f, 0.0f}
	, m_size{0.0f, 0.0f}
	, m_rotation(0.0f)
{

}

OBB::~OBB()
{
	
}

void OBB::CheckCollision()
{
	if (GetObbTarget() != nullptr)
	{
		SetCollide(CalculateCollision(*GetObbTarget()));
	}
}

void OBB::Render(DirectY& directY) const
{
	if (m_isCollide)
	{
		directY.SetBrushColor(D2D1::ColorF::Red);
	}
	else
	{
		directY.SetBrushColor(D2D1::ColorF::Green);
	}

	directY.DrawRectangle(D2D1_RECT_F
		{ 
			m_centerPos.m_x - m_size.m_x, 
			m_centerPos.m_y - m_size.m_y, 
			m_centerPos.m_x + m_size.m_x, 
			m_centerPos.m_y + m_size.m_y
		}, 
		2.8f);
}

bool OBB::CalculateCollision(const OBB& other) const
{
	Vector2 axes[4] =
	{
		{cos(m_rotation), sin(m_rotation)},
		{cos(m_rotation + (float)M_PI_2), sin(m_rotation + (float)M_PI_2)},
		{cos(other.GetRotation()), sin(other.GetRotation())},
		{cos(other.GetRotation() + (float)M_PI_2), sin(other.GetRotation() + (float)M_PI_2)}
	};

	for (int i = 0; i < 4; i++)
	{
		Vector2 t = other.GetCenterPos() - m_centerPos;

		float projection1 = Vector2 
		{
			axes[i].Dot({m_size.m_x, 0}),
			axes[i].Dot({0, m_size.m_y})
		}.Magnitude(); 
		
		float projection2 = Vector2
		{
			axes[i].Dot({other.GetSize().m_x, 0}),
			axes[i].Dot({0, other.GetSize().m_y})
		}.Magnitude();

		float distance = abs(t.Dot(axes[i]));

		if (distance > projection1 + projection2)
		{
			return false;
		}
	}

	return true;
}

