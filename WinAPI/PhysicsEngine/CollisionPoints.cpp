#include "CollisionPoints.h"

namespace PhysicsEngine
{
	void CollisionPoints::Reset()
	{
		a.Zero();
		b.Zero();
		normal.Zero();
		depth = 0.f;
		hasCollision = false;
	}

}