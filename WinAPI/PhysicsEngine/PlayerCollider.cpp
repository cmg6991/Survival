#include "PlayerCollider.h"

#include "DirectY.h"
#include "Constants.h"
#include "Transform.h"

namespace PhysicsEngine
{

	PlayerCollider::PlayerCollider(
		float x, float y, float h)
		: width(h * 2.f)
		, height(h)
		, leftTop()
		, leftDown()
		, rightTop()
		, rightDown()
	{
		this->center = MathEngine::Vector2(x, y);
		this->initCenter = MathEngine::Vector2(x, y);

		leftTop = MathEngine::Vector2(initCenter.x, initCenter.y - height / 2.f);
		leftDown = MathEngine::Vector2(initCenter.x - width / 2.f, initCenter.y);
		rightTop = MathEngine::Vector2(initCenter.x, initCenter.y + height / 2.f);
		rightDown = MathEngine::Vector2(initCenter.x + width / 2.f, initCenter.y);
	}

	PlayerCollider::PlayerCollider(MathEngine::Vector2 center, float h)
		: width(h * 2.f)
		, height(h)
		, leftTop()
		, leftDown()
		, rightTop()
		, rightDown()
	{
		this->center = center;
		this->initCenter = center;

		leftTop = MathEngine::Vector2(initCenter.x, initCenter.y - height / 2.f);
		leftDown = MathEngine::Vector2(initCenter.x - width / 2.f, initCenter.y);
		rightTop = MathEngine::Vector2(initCenter.x, initCenter.y + height / 2.f);
		rightDown = MathEngine::Vector2(initCenter.x + width / 2.f, initCenter.y);
	}

	CollisionPoints PlayerCollider::TestCollision(
		const Transform& transform, 
		const Collider& collider, 
		const Transform& colliderTransform) const
	{
		return collider.TestCollision(
			colliderTransform, *this, transform);
	}

	CollisionPoints PlayerCollider::TestCollision(
		const Transform& transform,
		const CircleCollider& circle,
		const Transform& circleTransform) const
	{
		return CollisionPoints();
	}

	CollisionPoints PlayerCollider::TestCollision(
		const Transform& transform,
		const RectangleCollider& rectangle,
		const Transform& rectangleTransform) const
	{
		return CollisionPoints();
	}


	CollisionPoints PlayerCollider::TestCollision(
		const Transform& transform,
		const IsometricCollider& isometric,
		const Transform& isometricTransform) const
	{
		return PlayerVsDiamond( *this,
			transform, isometric, isometricTransform);
	}

	void PlayerCollider::DrawCollider(
		DirectY& draw, const Transform& transform) const
	{
		draw.DrawLine(Graphics::BaseFigure::Line {leftTop.x, leftTop.y, leftDown.x ,leftDown.y }, 2.f);
		draw.DrawLine(Graphics::BaseFigure::Line {leftDown.x, leftDown.y, rightDown.x, rightDown.y}, 2.f);
		draw.DrawLine(Graphics::BaseFigure::Line {rightDown.x, rightDown.y, rightTop.x, rightTop.y}, 2.f);
		draw.DrawLine(Graphics::BaseFigure::Line {rightTop.x, rightTop.y, leftTop.x, leftTop.y}, 2.f);
	}

}