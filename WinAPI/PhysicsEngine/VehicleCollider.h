#pragma once

#include "Collider.h"

namespace PhysicsEngine
{
	struct VehicleCollider : public Collider
	{
		float width;
		float height;

		MathEngine::Vector2 points[4];

		VehicleCollider(float x, float y, float h, float w);
		VehicleCollider(MathEngine::Vector2 center, float h, float w);

		CollisionPoints TestCollision(const Collider& collider) const override;
		CollisionPoints TestCollision(const CircleCollider& circle) const override;
		CollisionPoints TestCollision(const RectangleCollider& rectangle) const override;
		CollisionPoints TestCollision(const IsometricCollider& isometric) const override;

		void Update() override;


	private:
		void SetSide();
		void SetLeftTop();
		void SetFront();
		void SetRightTop();
	};
}