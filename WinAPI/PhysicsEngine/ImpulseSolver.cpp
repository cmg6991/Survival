#include "ImpulseSolver.h"
#include "Vector2.h"
#include "Object.h"
#include "Collision.h"

namespace PhysicsEngine
{
	void ImpulseSolver::Solve(std::vector<Collision>& collisions, float dt)
	{
		for (Collision& collision : collisions)
		{
			Object* a = collision.objA;
			Object* b = collision.objB;

			if (a->isStatic && b->isStatic) continue;

			//if (a->isKinematic || b->isKinematic) continue;

			// 정적 오브젝트는 질량 무한대로 취급 (역질량 0)
			float invMassA = a->isStatic ? 0.f : 1.f / a->mass;
			float invMassB = b->isStatic ? 0.f : 1.f / b->mass;
			float invMassSum = invMassA + invMassB;
			if (invMassSum <= 0.f) continue;

			// 서로 다가가는 속도 성분만 계산
			MathEngine::Vector2 relativeVelocity = b->velocity - a->velocity;
			//athEngine::Vector2 relativeVelocity = a->velocity - b->velocity;
			float velAlongNormal = relativeVelocity.Dot(collision.points.normal);

			// 이미 서로 멀어지고 있으면 튕길 필요 없음
			if (velAlongNormal > 0.f) continue;

			const float restitution = 0.1f; // 0 = 반발 없이 밀기만, 1 = 완전 탄성 튕김

			float j = -(1.f + restitution) * velAlongNormal;
			j /= invMassSum;

			MathEngine::Vector2 impulse = collision.points.normal * j;

			if (!a->isStatic) a->velocity -= impulse * invMassA;
			if (!b->isStatic) b->velocity += impulse * invMassB;
		}
		// collisions.clear()는 여기서 하지 않음 - PositionSolver가 마지막에 정리
	}
}
