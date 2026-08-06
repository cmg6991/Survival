#pragma once

#include "Object.h"
#include "CollisionPoints.h"

namespace PhysicsEngine
{
	struct Collision
	{
		Object* objA;
		Object* objB;
		CollisionPoints points;

		Collision
		(
			Object* objA, Object* objB,
			const CollisionPoints& collisionPoints
		);
	};
}
