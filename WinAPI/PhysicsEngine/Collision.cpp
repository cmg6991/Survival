#include "Collision.h"

namespace PhysicsEngine
{

	Collision::Collision(
		Object* objA, 
		Object* objB, 
		const CollisionPoints& collisionPoints)
		: objA(objA)
		, objB(objB)
		, points(collisionPoints)
	{

	}
}