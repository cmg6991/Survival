#include "RectangleCollider.h"

#include "CircleCollider.h"

namespace PhysicsEngine
{

	RectangleCollider::RectangleCollider(
		float x, float y, float w, float h)
		: size(w, h)
	{
		this->center = MathEngine::Vector2(x, y);
	}

	RectangleCollider::RectangleCollider(
		MathEngine::Vector2 center, 
		MathEngine::Vector2 size)
		: size(size)
	{
		this->center = MathEngine::Vector2(center);
	}

	CollisionPoints RectangleCollider::TestCollision(const Collider& collider) const
	{
		return collider.TestCollision(*this);
	}

	CollisionPoints RectangleCollider::TestCollision(const CircleCollider& circle) const
	{
		return CircleVsRectangle(circle, *this);
	}

	CollisionPoints RectangleCollider::TestCollision(const RectangleCollider& rectangle) const
	{
		return RectangleVsRectangle(*this, rectangle);
	}

	CollisionPoints RectangleCollider::TestCollision(const IsometricCollider& isometric) const
	{
		return CollisionPoints();
	}

	void RectangleCollider::Update()
	{

	}
};