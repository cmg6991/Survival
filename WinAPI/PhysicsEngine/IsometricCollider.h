///
/// 아이소메트릭의 마름모 충돌체
/// 각도 60, 120, 60, 120 으로
/// 

#pragma once

#include "Collider.h"

namespace PhysicsEngine 
{

	struct IsometricCollider : public Collider
	{
		float width;
		float height;

		IsometricCollider(float x, float y);
		IsometricCollider(float x, float y, float h);
		IsometricCollider(MathEngine::Vector2 center, float h);

		CollisionPoints TestCollision(const Collider& collider) const override;
		CollisionPoints TestCollision(const CircleCollider& circle) const override;
		CollisionPoints TestCollision(const RectangleCollider& rectangle) const override;
		CollisionPoints TestCollision(const IsometricCollider& isometric) const override;

		void Update() override;
	};
}