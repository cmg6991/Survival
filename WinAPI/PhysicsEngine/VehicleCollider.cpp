#include "VehicleCollider.h"

#include "Constants.h"

namespace PhysicsEngine
{

	VehicleCollider::VehicleCollider(float x, float y, float h, float w)
		: width(w)
		, height(h)
		, points()
	{
		this->center = MathEngine::Vector2(x, y);
		this->initCenter = MathEngine::Vector2(x, y);

		SetSide();
	}

	VehicleCollider::VehicleCollider(MathEngine::Vector2 center, float h, float w)
		: width(w)
		, height(h)
		, points()
	{
		this->center = center;
		this->initCenter = center;

		SetSide();
	}

	CollisionPoints VehicleCollider::TestCollision(const Collider& collider) const
	{
		return collider.TestCollision(*this);
	}

	CollisionPoints VehicleCollider::TestCollision(const CircleCollider& circle) const
	{
		return CollisionPoints();
	}

	CollisionPoints VehicleCollider::TestCollision(const RectangleCollider& rectangle) const
	{
		return CollisionPoints();
	}

	CollisionPoints VehicleCollider::TestCollision(const IsometricCollider& isometric) const
	{
		return VehicleVsDiamond( *this, isometric);
	}

	void VehicleCollider::Update()
	{

		if (000.f <= m_degree && m_degree < 030.f || 180.f <= m_degree && m_degree < 210.f) SetSide();
		if (030.f <= m_degree && m_degree < 065.f || 210.f <= m_degree && m_degree < 245.f) SetLeftTop();

		if (065.f <= m_degree && m_degree < 115.f || 245.f <= m_degree && m_degree < 295.f) SetFront();

		if (115.f <= m_degree && m_degree < 150.f || 295.f <= m_degree && m_degree < 330.f) SetRightTop();
		if (150.f <= m_degree && m_degree < 180.f || 330.f <= m_degree && m_degree < 360.f) SetSide();
	}

	void VehicleCollider::SetSide()
	{
		points[0] = MathEngine::Vector2(initCenter.x - width * 0.25f, initCenter.y - height * 0.0f);
		points[1] = MathEngine::Vector2(initCenter.x - width * 0.25f, initCenter.y + height * 0.35f);
		points[2] = MathEngine::Vector2(initCenter.x + width * 0.25f, initCenter.y + height * 0.35f);
		points[3] = MathEngine::Vector2(initCenter.x + width * 0.25f, initCenter.y - height * 0.0f);
	}

	void VehicleCollider::SetLeftTop() 
	{
		points[0] = MathEngine::Vector2(initCenter.x - width * 0.1f, initCenter.y - height * 0.2f);
		points[1] = MathEngine::Vector2(initCenter.x - width * 0.3f, initCenter.y + height * 0.0f);
		points[2] = MathEngine::Vector2(initCenter.x + width * 0.05f, initCenter.y + height * 0.5f);
		points[3] = MathEngine::Vector2(initCenter.x + width * 0.25f, initCenter.y + height * 0.3f);
	}

	void VehicleCollider::SetFront()
	{
		points[0] = MathEngine::Vector2(initCenter.x - width * 0.15f, initCenter.y - height * 0.25f);
		points[1] = MathEngine::Vector2(initCenter.x - width * 0.15f, initCenter.y + height * 0.4f);
		points[2] = MathEngine::Vector2(initCenter.x + width * 0.15f, initCenter.y + height * 0.4f);
		points[3] = MathEngine::Vector2(initCenter.x + width * 0.15f, initCenter.y - height * 0.25f);
	}

	void VehicleCollider::SetRightTop()
	{
		points[0] = MathEngine::Vector2(initCenter.x + width * 0.1f, initCenter.y - height * 0.2f);
		points[1] = MathEngine::Vector2(initCenter.x - width * 0.25f, initCenter.y + height * 0.3f);
		points[2] = MathEngine::Vector2(initCenter.x - width * 0.05f, initCenter.y + height * 0.5f);
		points[3] = MathEngine::Vector2(initCenter.x + width * 0.3f, initCenter.y + height * 0.0f);
	}
}