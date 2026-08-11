///
/// 물리적인 특성을 가진 구조체
///

#pragma once

#include "Vector2.h"
#include "Collider.h"
#include <functional>
#include <unordered_set>

namespace PhysicsEngine
{
	struct Object
	{
		Object(
			MathEngine::Vector2 p,
			MathEngine::Vector2 v = MathEngine::Vector2(0.f, 0.f),
			MathEngine::Vector2 f = MathEngine::Vector2(0.f, 0.f),
			float m = 1.f,
			bool isStatic = false);
		Object(MathEngine::Vector2 p, float m); 

		~Object();

		/// Rigidbody
		
		// 위치
		MathEngine::Vector2 position;
		// 속도
		MathEngine::Vector2 velocity;
		// 힘
		MathEngine::Vector2 force;
		// 질량
		float mass;

		bool isStatic=false;			//정적 오브젝트 

		bool isKinematic = false;		//플레이어한테 kinematic

		bool isColliding = false;		//충돌 처리 
		bool isEnabled = true;			//오브젝트 풀 전용함수
		bool isTrigger = false;

		// 충돌체
		Collider* collider;

		void* owner = nullptr;

		std::function<void(Object* other)> onCollisionEnter;
		std::function<void(Object* other)> onCollisionStay;
		std::function<void(Object* other)> onCollisionExit;

		std::unordered_set<Object*> currentCollisions;
		std::unordered_set<Object*> previousCollisions;
	};
}