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

			if (obj->isKinematic)
			{
				// 위치는 이미 Pull에서 반영됐으니 Step에서는 이동시키지 않음
				obj->collider->center = obj->position;
				continue;
			}

			obj->velocity += obj->force / obj->mass * dt;
			obj->position += obj->velocity * dt;
			obj->velocity *= 0.8f;
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
		//std::vector<Collision> collisions;

		//for (Object* obj : m_objects)
		//{
		//	if (obj) obj->isColliding = false;
		//}

		//size_t count = m_objects.size();
		//for (size_t i = 0; i < count; ++i)
		//{
		//	Object* a = m_objects[i];
		//	if (!a || !a->collider) continue;

		//	for (size_t j = i + 1; j < count; ++j)
		//	{
		//		Object* b = m_objects[j];
		//		if (!b || !b->collider) continue;

		//		if (a->isStatic && b->isStatic) continue;

		//		CollisionPoints points = a->collider->TestCollision(*b->collider);

		//		if (points.hasCollision)
		//		{
		//			// 충돌 감지 확인용 디버그 출력 (Visual Studio '출력' 창에 표시됨)
		//			OutputDebugString(L"출력");

		//			collisions.emplace_back(a, b, points);
		//			//TestResolve(*a, *b, points);

		//			a->isColliding = true;   // 추가
		//			b->isColliding = true;   // 추가
		//		}
		//	}
		//}

		//ResolveCollsion(collisions, dt);

		std::vector<Collision> collisions;

		for (Object* obj : m_objects)
		{
			if (!obj) continue;
			obj->isColliding = false;
			obj->previousCollisions = obj->currentCollisions;
			obj->currentCollisions.clear();
		}

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
					collisions.emplace_back(a, b, points);
					a->isColliding = true;
					b->isColliding = true;

					a->currentCollisions.insert(b);
					b->currentCollisions.insert(a);

					bool wasCollidingBefore = (a->previousCollisions.find(b) != a->previousCollisions.end());

					if (wasCollidingBefore)
					{
						if (a->onCollisionStay) a->onCollisionStay(b);
						if (b->onCollisionStay) b->onCollisionStay(a);
					}
					else
					{
						if (a->onCollisionEnter) a->onCollisionEnter(b);
						if (b->onCollisionEnter) b->onCollisionEnter(a);
					}
				}
			}
		}

		for (Object* obj : m_objects)
		{
			if (!obj) continue;
			for (Object* prevOther : obj->previousCollisions)
			{
				if (obj->currentCollisions.find(prevOther) == obj->currentCollisions.end())
				{
					if (obj->onCollisionExit) obj->onCollisionExit(prevOther);
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

		for (Object* other : m_objects)
		{
			if (other == object) continue;
			other->currentCollisions.erase(object);
			other->previousCollisions.erase(object);
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

	//void PhysicsWorld::TestResolve(
	//	Object& a, Object& b, const CollisionPoints& points)
	//{
	//	if (a.isStatic && b.isStatic) return;

	//	const float slop = 0.01f;      // 이 정도 겹침은 무시 (부동소수점 떨림 방지)
	//	const float percent = 0.8f;    // 한 프레임에 겹침의 80%만 보정 (100%면 지금처럼 순간이동)

	//	float correctionDepth = points.depth - slop;
	//	if (correctionDepth <= 0.f) return; // 겹침이 미미하면 아예 보정 안 함

	//	MathEngine::Vector2 correction = points.normal * (correctionDepth * percent);

	//	if (a.isStatic)
	//	{
	//		b.position -= correction;
	//	}
	//	else if (b.isStatic)
	//	{
	//		a.position += correction;
	//	}
	//	else
	//	{
	//		a.position += correction * 0.5f;
	//		b.position -= correction * 0.5f;
	//	}

	//	a.collider->center = a.position;
	//	b.collider->center = b.position;
	//}
}