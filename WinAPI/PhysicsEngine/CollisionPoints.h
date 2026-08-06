///
/// 두 개의 객체가 충돌한 지점과 
/// 충돌 관련 정보를 담은 구조체
/// 

#pragma once

#include "../MathEngine/Vector2.h"

namespace PhysicsEngine
{
	struct CollisionPoints
	{
		// 객체 A 의 충돌 위치 정보
		MathEngine::Vector2 a = (0.f, 0.f);
		// 객체 B 의 충돌 위치 정보
		MathEngine::Vector2 b = (0.f, 0.f);
		// 두 객체 사이의 충돌 법선 벡터
		MathEngine::Vector2 normal = (0.f, 0.f);
		// 두 객체의 충돌 정도
		float depth = 0.0f;
		// 충돌 발생 여부
		bool hasCollision = false;

	public:
		// 충돌하지 않은 상태로 초기화
		void Reset();
	};
}