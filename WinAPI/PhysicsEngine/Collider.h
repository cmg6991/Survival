///
/// 충돌체 기본 클래스 및 인터페이스와 
/// 
/// 충돌 알고리즘 <- 여기서 이걸 갖고 있는게 매우 맘에 안 듦
/// 

#pragma once

#include "CollisionPoints.h"

namespace PhysicsEngine
{
	struct CircleCollider;
	struct RectangleCollider;
	struct IsometricCollider;
	struct VehicleCollider;

	struct Collider
	{
		MathEngine::Vector2 center;
		MathEngine::Vector2 initCenter;
		float m_degree = 0.f;

		virtual ~Collider() {};

		//  Collider 를 상속받는 다른 충돌체들과의 충돌 검사
		virtual CollisionPoints TestCollision 
		(const Collider& collider) const = 0;

		// CircleCollider 와의 충돌 검사
		virtual CollisionPoints TestCollision
		(const CircleCollider& circle) const = 0;

		// RectangleCollider 와의 충돌 검사
		virtual CollisionPoints TestCollision
		(const RectangleCollider& rectangle) const = 0;

		// IsometricCollider 와의 충돌 검사
		virtual CollisionPoints TestCollision
		(const IsometricCollider& isometric) const = 0;

		virtual void Update() = 0;

	protected:
		/// 원 대 원
		CollisionPoints CircleVsCircle
		(const CircleCollider& a, const CircleCollider& b) const;

		/// 원 대 네모
		CollisionPoints CircleVsRectangle
		(const CircleCollider& a, const RectangleCollider& b) const;

		/// 네모 대 네모
		CollisionPoints RectangleVsRectangle
		(const RectangleCollider& a, const RectangleCollider& b) const;
		
		/// 마름모 대 마름모 AABB
		CollisionPoints DiamondVsDiamond
		(const IsometricCollider& a, const IsometricCollider& b) const;

		/// 플레이어 대 마름모 Obb
		CollisionPoints VehicleVsDiamond (
			const VehicleCollider& v, const IsometricCollider& d) const;
	};
}