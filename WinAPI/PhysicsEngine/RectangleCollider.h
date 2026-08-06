#pragma once

#include "Collider.h"

namespace PhysicsEngine
{
	struct RectangleCollider : public Collider
	{
		MathEngine::Vector2 size;

		RectangleCollider(float x, float y, float w, float h);
		RectangleCollider(MathEngine::Vector2 center,MathEngine::Vector2 size);

		CollisionPoints TestCollision(const Collider& collider) const override;
		CollisionPoints TestCollision(const CircleCollider& circle) const override;
		CollisionPoints TestCollision(const RectangleCollider& rectangle) const override;
		CollisionPoints TestCollision(const IsometricCollider& isometric) const override;

		void Update() override;

	};
}