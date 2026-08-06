#include "CircleCollider.h"

#include "RectangleCollider.h"
#include "Constants.h"

namespace PhysicsEngine
{

	CircleCollider::CircleCollider(float x, float y, float r)
		: radius(r)
	{
		this->center = MathEngine::Vector2(x, y);
	}

	CircleCollider::CircleCollider(MathEngine::Vector2 center, float r)
		: radius(r)
	{
		this->center = center;
	}

	CollisionPoints CircleCollider::TestCollision(const Collider& collider) const
	{
		return collider.TestCollision(*this);
	}

	CollisionPoints CircleCollider::TestCollision(const CircleCollider& circle) const
	{
		return CircleVsCircle(*this, circle);
	}

	CollisionPoints CircleCollider::TestCollision(const RectangleCollider& rectangle) const
	{
		return CircleVsRectangle(*this, rectangle);
	}

	CollisionPoints CircleCollider::TestCollision(const IsometricCollider& isometric) const
	{
		return CollisionPoints();
	}

	void CircleCollider::Update()
	{

	}
}