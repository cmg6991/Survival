#include "PositionSolver.h"

#include "Vector2.h"
#include "Object.h"
#include "Collision.h"

namespace PhysicsEngine
{
	void PositionSolver::Solve(
		std::vector<Collision>& collisions, float dt)
	{
		/*
		for (Collision& collsion : collisions)
		{
			Object* aBody = collsion.objA;
			Object* bBody = collsion.objB;

			MathEngine::Vector2 resolution =
				collsion.points.normal *
				collsion.points.depth;

			// 이런다고 반영되지 않는다.
			aBody->transform->position -= resolution;
			bBody->transform->position += resolution;
		}
		
		collisions.clear();
		*/

		/*const float slop = 0.001f;
		const float percent = 0.4f;

		for (Collision& collision : collisions)
		{
			Object* a = collision.objA;
			Object* b = collision.objB;

			if (a->isStatic && b->isStatic) continue;

			float correctionDepth = collision.points.depth - slop;
			if (correctionDepth <= 0.f) continue;

			MathEngine::Vector2 correction =
				collision.points.normal * (correctionDepth * percent);

			if (a->isStatic)
			{
				b->position -= correction;
				b->collider->center = b->position;
			}
			else if (b->isStatic)
			{
				a->position += correction;
				a->collider->center = a->position;
			}
			else
			{
				float totalMass = a->mass + b->mass;
				float aRatio = b->mass / totalMass;
				float bRatio = a->mass / totalMass;

				a->position += correction * aRatio;
				b->position -= correction * bRatio;
				a->collider->center = a->position;
				b->collider->center = b->position;
			}
		}

		collisions.clear();*/
		const float slop = 0.001f;
		const float percent = 0.4f;

		for (Collision& collision : collisions)
		{
			Object* a = collision.objA;
			Object* b = collision.objB;

			if (a->isStatic && b->isStatic) continue;

			// ★ kinematic도 static처럼 "안 밀리는 쪽"으로 취급
			bool aFixed = a->isStatic || a->isKinematic;
			bool bFixed = b->isStatic || b->isKinematic;

			if (aFixed && bFixed) continue;

			float correctionDepth = collision.points.depth - slop;
			if (correctionDepth <= 0.f) continue;

			MathEngine::Vector2 correction = collision.points.normal * (correctionDepth * percent);

			if (aFixed)
			{
				b->position += correction;
				b->collider->center = b->position;
			}
			else if (bFixed)
			{
				a->position -= correction;
				a->collider->center = a->position;
			}
			else
			{
				float totalMass = a->mass + b->mass;
				float aRatio = b->mass / totalMass;
				float bRatio = a->mass / totalMass;
				a->position -= correction * aRatio;
				b->position += correction * bRatio;
				a->collider->center = a->position;
				b->collider->center = b->position;
			}
		}
		collisions.clear();
	}
}
