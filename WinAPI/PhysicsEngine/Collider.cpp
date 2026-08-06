#include "Collider.h"

#include "CircleCollider.h"
#include "RectangleCollider.h"
#include "IsometricCollider.h"
#include "VehicleCollider.h"

#include "CollisionPoints.h"

#include "MathUtils.h"
#include <algorithm>

namespace PhysicsEngine
{
	CollisionPoints Collider::CircleVsCircle
	(const CircleCollider& a,const CircleCollider& b) const
	{
		CollisionPoints result;

		MathEngine::Vector2 direction = b.center - a.center;
		float distance = direction.Magnitude();
		float radii = a.radius + b.radius;

		// 충돌 검사
		if (distance <= radii)
		{
			// 충돌 지점 계산
			float depth = radii - distance;
			MathEngine::Vector2 nomal = direction.Normalize();
			MathEngine::Vector2 collisionPointA =
				a.center + nomal * (a.radius - depth);
			
			MathEngine::Vector2 collisionPointB =
				b.center - nomal * (b.radius - depth);

			// 결과 설정
			result.a = collisionPointA;
			result.b = collisionPointB;
			result.normal = nomal;
			result.depth = depth;
			result.hasCollision = true;
		}
		else
		{
			// 충돌하지 않은 경우
			result.Reset();
		}

		return result;
	}

	CollisionPoints Collider::CircleVsRectangle
	(const CircleCollider& a, const RectangleCollider& b) const
	{
		CollisionPoints result;

		MathEngine::Vector2 halfSizeB = b.size / 2.f;

		// OBB와 원의 충돌 검사
		float closestX = std::max(std::min(a.center.x, b.center.x + halfSizeB.x), b.center.x - halfSizeB.x);
		float closestY = std::max(std::min(a.center.y, b.center.y + halfSizeB.y), b.center.y - halfSizeB.y);
		MathEngine::Vector2 closestPoint = { closestX, closestY };
		MathEngine::Vector2 distance = a.center.Distance(closestPoint);

		// 충돌 검사 결과 설정

		// 충돌 지점 계산
		if (distance.Magnitude() < a.radius)
		{
			result.a = closestPoint;
			result.b = closestPoint;
			result.normal = (closestPoint - a.center).Normalize();
			// 원과 OBB의 겹침 정도 계산
			result.depth = 1.f * a.radius - distance.Magnitude();
			result.hasCollision = true;
		}

		return result;
	}

	CollisionPoints Collider::RectangleVsRectangle
	(const RectangleCollider& a, const RectangleCollider& b) const
	{
		CollisionPoints result;


		// 두 OBB의 충돌 법선 계산
		MathEngine::Vector2 axesA[2] =
		{
			MathEngine::Vector2
			{
				cosf(MathEngine::DEGREE_TO_RADIAN(0.f)),
				sinf(MathEngine::DEGREE_TO_RADIAN(0.f))
			},
			MathEngine::Vector2
			{
				cosf(MathEngine::DEGREE_TO_RADIAN(90.f)),
				sinf(MathEngine::DEGREE_TO_RADIAN(90.f))
			}
		};

		MathEngine::Vector2 axesB[2] =
		{
			MathEngine::Vector2
			{
				cosf(MathEngine::DEGREE_TO_RADIAN(0.f)),
				sinf(MathEngine::DEGREE_TO_RADIAN(0.f))
			},
			MathEngine::Vector2
			{
				cosf(MathEngine::DEGREE_TO_RADIAN(90.f)),
				sinf(MathEngine::DEGREE_TO_RADIAN(90.f))
			}
		};

		// 두 OBB의 충돌 법선 계산
		for (int i = 0; i < 2; i++)
		{
			float projectionA[2] =
			{ a.center * axesA[i], a.size * axesA[i] };
			float projectionB[2] =
			{ b.center * axesA[i], b.size * axesA[i] };

			float sumA = projectionA[0] + 0.5f * projectionA[1];
			float sumB = projectionB[0] + 0.5f * projectionB[1];
			float diffA = projectionA[0] - 0.5f * projectionA[1];
			float diffB = projectionB[0] - 0.5f * projectionB[1];

			if (sumA < diffB || sumB < diffA)
			{
				result.Reset();
				return result;
			}
		}

		for (int i = 0; i < 2; i++)
		{
			float projectionA[2] = 
			{
				a.center* axesB[i],
				a.size* axesB[i]
			};
			float projectionB[2] =
			{
				b.center* axesB[i],
				b.size * axesB[i]
			};

			float sumA = projectionA[0] + 0.5f * projectionA[1];
			float sumB = projectionB[0] + 0.5f * projectionB[1];
			float diffA = projectionA[0] - 0.5f * projectionA[1];
			float diffB = projectionB[0] - 0.5f * projectionB[1];

			if (sumA < diffB || sumB < diffA)
			{
				result.Reset();
				return result;
			}
		}

		// 충돌 지점 계산
		MathEngine::Vector2 direction = a.center - b.center;
		MathEngine::Vector2 normal = direction.Normalize();

		MathEngine::Vector2 halfSizeA = a.size / 2.f;
		MathEngine::Vector2 halfSizeB = b.size / 2.f;
		MathEngine::Vector2 collisionPointA = a.center + normal * halfSizeA;
		MathEngine::Vector2 collisionPointB = b.center - normal * halfSizeB;
		MathEngine::Vector2 collisionPoin = (collisionPointA + collisionPointB) / 2.f;

		float distance = direction.Magnitude();
		float depth = distance - (a.size.Magnitude() + b.size.Magnitude()) / 2.f ;

		// 결과 설정
		result.a = collisionPoin;
		result.b = collisionPoin;
		result.normal = normal;
		result.depth = depth;
		result.hasCollision = true;

		return result;
	}

	CollisionPoints Collider::DiamondVsDiamond
	(const IsometricCollider& a, const IsometricCollider& b) const
	{
		CollisionPoints result;

		// 꼭짓점
		MathEngine::Vector2 topA = MathEngine::Vector2(a.center.x, a.center.y - a.height / 2.f);
		MathEngine::Vector2 bottomA = MathEngine::Vector2(a.center.x, a.center.y + a.height / 2.f);
		MathEngine::Vector2 topB = MathEngine::Vector2(b.center.x, b.center.y - b.height / 2.f);
		MathEngine::Vector2 bottomB = MathEngine::Vector2(b.center.x, b.center.y + b.height / 2.f);

		// y 절편
		float m = 0.5f;
		float yminA = topA.y - m * topA.x;
		float ymaxA = bottomA.y - m * bottomA.x;
		float yminB = topB.y - m * topB.x;
		float ymaxB = bottomB.y - m * bottomB.x;

		// x 절편
		m = - 0.5f;
		float xminA = - (topA.y - m * topA.x) / m;
		float xmaxA = - (bottomA.y - m * bottomA.x) / m;
		float xminB = - (topB.y - m * topB.x) / m;
		float xmaxB = - (bottomB.y - m * bottomB.x) / m;

		if (((ymaxA > yminB) && (ymaxA < ymaxB) || (yminA < ymaxB) && (ymaxA > ymaxB))&&
			((xmaxA > xminB) && (xmaxA < xmaxB) || (xminA < xmaxB) && (xmaxA > xmaxB)))
		{
			MathEngine::Vector2 direction = a.center - b.center;
		
			// 결과 설정
			result.a = a.center;
			result.b = b.center;
			result.depth = direction.Magnitude();
			result.normal = direction.Normalize();
			result.hasCollision = true;
		}
		
		return result;
	}

	// 다각형 충돌을 이용
	// 참고 : https://codeonwort.tistory.com/139
	PhysicsEngine::CollisionPoints Collider::VehicleVsDiamond
	(const VehicleCollider& v, const IsometricCollider& d) const
	{
		PhysicsEngine::CollisionPoints result;

		MathEngine::Vector2 vPoints[4];
		vPoints[0] = v.points[0] + v.center;
		vPoints[1] = v.points[1] + v.center;
		vPoints[2] = v.points[2] + v.center;
		vPoints[3] = v.points[3] + v.center;

		MathEngine::Vector2 dPoints[4];
		dPoints[0] = MathEngine::Vector2{ d.center.x, d.center.y - d.height / 2.f };
		dPoints[1] = MathEngine::Vector2{ d.center.x + d.width / 2, d.center.y };
		dPoints[2] = MathEngine::Vector2{ d.center.x, d.center.y + d.height / 2.f };
		dPoints[3] = MathEngine::Vector2{ d.center.x - d.width / 2, d.center.y};
		
		MathEngine::Vector2 edges[8];
		edges[0] = vPoints[0] - vPoints[1];
		edges[1] = vPoints[1] - vPoints[2];
		edges[2] = vPoints[2] - vPoints[3];
		edges[3] = vPoints[3] - vPoints[0];

		edges[4] = dPoints[0] - dPoints[1];
		edges[5] = dPoints[1] - dPoints[2];
		edges[6] = dPoints[2] - dPoints[3];
		edges[7] = dPoints[3] - dPoints[0];

		MathEngine::Vector2 edge;
		for (int i = 0; i < 8; ++i) 
		{
			// 현재 변에 수직인 축
			MathEngine::Vector2 axis = MathEngine::Vector2{ -edges[i].y, edges[i].x };
			axis.Normalize();

			float minA = 0.f;
			float maxA = 0.f;
			float minB = 0.f;
			float maxB = 0.f;

			// 정사형
			auto Projection = [](
				MathEngine::Vector2 axis,
				MathEngine::Vector2 points[4],
				float& minResult, float& maxResult)
			{
				float dotProduct = axis * points[0];
				minResult = dotProduct;
				maxResult = dotProduct;

				for (int i = 0; i < 4; i++)
				{
					dotProduct = points[i] * axis;
					if (dotProduct < minResult) minResult = dotProduct;
					else if (dotProduct > maxResult) maxResult = dotProduct;
				}
			};

			Projection(axis, vPoints, minA, maxA);
			Projection(axis, dPoints, minB, maxB);

			// 거리 검사
			auto IntervalDistance = [](float minA, float maxA, float minB, float maxB)
			{
				if (minA < minB) return minB - maxA;
				else return minA - maxB;
			};

			if (IntervalDistance(minA, maxA, minB, maxB) > 0.f)
			{
				result.hasCollision = false;
				return result;
			}
		}

		// 결과 설정
		MathEngine::Vector2 direction = v.center - d.center;
		result.depth = direction.Magnitude();
		result.normal = direction.Normalize();
		result.hasCollision = true;

		result.hasCollision = true;

		return result;
	}
}