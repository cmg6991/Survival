#pragma once

#include <vector>
#include "Collision.h"

namespace PhysicsEngine
{
	class Solver
	{
	public:
		virtual void Solve
		(std::vector<Collision>& collisions
			, float dt) = 0;
	};
}
