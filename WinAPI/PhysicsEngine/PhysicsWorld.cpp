#include "PhysicsWorld.h"

#include "Object.h"
#include "Solver.h"
#include "Collision.h"

#include <windowsx.h>
#include <windows.h>

namespace PhysicsEngine
{
	PhysicsWorld::PhysicsWorld()
		: m_objects{}
		, m_solvers{}
		, m_gravity(0.f, 0.f)
		, m_ScreenWidth(1920.f)
		, m_ScreenHeight(1080.f)
	{

	}

	PhysicsWorld::~PhysicsWorld()
	{
		for (Object* obj : m_objects)
		{
			delete obj;
		}
		m_objects.clear();

		for (Solver* solver : m_solvers)
		{
			delete solver;
		}
		m_solvers.clear();
	}

	void PhysicsWorld::Step(float dt)
	{
		for (Object* obj : m_objects)
		{
			if (obj->isStatic)
			{
				obj->collider->center = obj->position;
				continue;
			}

			obj->velocity += obj->force / obj->mass * dt;
			obj->position += obj->velocity * dt;
			obj->collider->center = obj->position;

			if (obj->velocity.Magnitude() < 0.001f)
			{
				obj->velocity = MathEngine::Vector2(0, 0);
			}

			obj->force = MathEngine::Vector2(0, 0); // 힘 리셋
		}
	}

	// 충돌 해결 하는 일 없음
	void PhysicsWorld::ResolveCollsion(
		std::vector<Collision>& collisions, float dt)
	{
		for (Solver* solver : m_solvers)
		{
			solver->Solve(collisions, dt);
		}
	}

	void PhysicsWorld::DetectCollision(float dt)
	{
		std::vector<Collision> collisions;

		size_t count = m_objects.size();
		for (size_t i = 0; i < count; ++i)
		{
			Object* a = m_objects[i];
			if (!a || !a->collider) continue;

			for (size_t j = i + 1; j < count; ++j)
			{
				Object* b = m_objects[j];
				if (!b || !b->collider) continue;

				if (a->isStatic && b->isStatic) continue;

				CollisionPoints points = a->collider->TestCollision(*b->collider);

				if (points.hasCollision)
				{
					// 충돌 감지 확인용 디버그 출력 (Visual Studio '출력' 창에 표시됨)
					OutputDebugString(L"출력");

					collisions.emplace_back(a, b, points);
					TestResolve(*a, *b, points);
				}
			}
		}

		ResolveCollsion(collisions, dt);
	}

	void PhysicsWorld::AddObject(Object* object)
	{
		m_objects.push_back(object);
	}

	void PhysicsWorld::RemoveObject(Object* object)
	{
		if (!object)
		{
			return;
		}

		auto itr = std::find
		(
			m_objects.begin(), m_objects.end(), object
		);

		if (itr == m_objects.end())
		{
			return;
		}

		delete* itr;

		m_objects.erase(itr);
	}

	void PhysicsWorld::AddSolver(Solver* solver)
	{
		m_solvers.push_back(solver);
	}

	void PhysicsWorld::RemoveSolver(Solver* solver)
	{
		if (!solver)
		{
			return;
		}

		auto itr = std::find
		(
			m_solvers.begin(), m_solvers.end(), solver
		);

		if (itr == m_solvers.end())
		{
			return;
		}
		
		delete *itr;

		m_solvers.erase(itr);
	}

	void PhysicsWorld::TestResolve(
		Object& a, Object& b, const CollisionPoints& points)
	{
		float dis = a.position.Distance(b.position);
		a.position +=
			points.normal * points.depth / dis;
		b.position -=
			points.normal * points.depth / dis;
	}

}