#pragma once

#include "Solver.h"

namespace PhysicsEngine
{ 
	class PositionSolver
		: public Solver
	{
		void Solve(
			std::vector<Collision>& collisions, 
			float dt) override;
	};
}