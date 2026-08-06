#pragma once

#include "Collider.h"

namespace PhysicsEngine
{
	struct CircleCollider : public Collider
	{
		float radius;

		CircleCollider(float x, float y, float r);
		CircleCollider(MathEngine::Vector2 center, float r);

		CollisionPoints TestCollision(const Collider& collider) const override;
		CollisionPoints TestCollision(const CircleCollider& circle) const override;
		CollisionPoints TestCollision(const RectangleCollider& rectangle) const override;
		CollisionPoints TestCollision(const IsometricCollider& isometric) const override;

		void Update() override;

	};
}