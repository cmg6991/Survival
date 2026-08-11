#pragma once

#include <vector>
#include <memory>

#include "../MathEngine/Vector2.h"

namespace PhysicsEngine
{
	struct Object;
	class Solver;
	struct Collision;
	struct CollisionPoints;
	struct Collider;

	class PhysicsWorld
	{
	public:
		PhysicsWorld();
		~PhysicsWorld();

	public:
		// 물리객체를 업데이트한다
		void Step(float dt);

		// 층돌 처리
		void ResolveCollsion(
			std::vector<Collision>& collisions, 
			float dt);

		void DetectCollision(float dt);

		void AddObject(Object* object);
		void RemoveObject(Object* object);

		void AddSolver(Solver* solver);
		void RemoveSolver(Solver* solver);

		void SetScreenSize(float w, float h)
		{
			m_ScreenWidth = w;
			m_ScreenHeight = h;
		}

		bool IsColliderBlocked(const Collider& testCollider) const;

	private:
		std::vector<Object*> m_objects;
		std::vector<Solver*> m_solvers;

		// 중력 가속도 
		MathEngine::Vector2 m_gravity;

		// 화면 크기
		float m_ScreenWidth;
		float m_ScreenHeight;

		/*void TestResolve(
			Object& a, Object& b,
			const CollisionPoints& points);*/
	};
}