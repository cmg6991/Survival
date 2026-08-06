#pragma once

#include "Collider.h"

namespace PhysicsEngine
{
	struct PlayerCollider : public Collider
	{
		float width;
		float height;

		MathEngine::Vector2 leftTop;
		MathEngine::Vector2 leftDown;
		MathEngine::Vector2 rightTop;
		MathEngine::Vector2 rightDown;

		PlayerCollider(float x, float y, float h);
		PlayerCollider(MathEngine::Vector2 center, float h);

		CollisionPoints TestCollision
		(
			const Transform& transform,
			const Collider& collider,
			const Transform& colliderTransform
		) const override;

		CollisionPoints TestCollision
		(
			const Transform& transform,
			const CircleCollider& circle,
			const Transform& circleTransform
		) const override;

		virtual CollisionPoints TestCollision
		(
			const Transform& transform,
			const RectangleCollider& rectangle,
			const Transform& rectangleTransform
		) const override;

		virtual CollisionPoints TestCollision
		(
			const Transform& transform,
			const IsometricCollider& isometric,
			const Transform& isometricTransform
		) const override;

		virtual void DrawCollider(
			DirectY& draw, const Transform& transform) const override;
	};
}