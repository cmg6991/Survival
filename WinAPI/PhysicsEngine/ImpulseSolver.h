#pragma once

#include "Solver.h"

namespace PhysicsEngine
{
	class ImpulseSolver : public Solver
	{
		void Solve(
			std::vector<Collision>& collisions,
			float dt) override;
	};
}