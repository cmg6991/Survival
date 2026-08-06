#include "FindPosition.h"

namespace Algorithm 
{
	void FindPosition::Insert(
		const MathEngine::Vector2 coordiation, 
		const MathEngine::Vector2 location)
	{
		m_mapData[coordiation] = location;
	}

	void FindPosition::Init()
	{
		MathEngine::Vector2 center = { 72.f, 108.f };
		MathEngine::Vector2 nowCenter;

		for (int x = 0; x < 100; ++x)
		{
			for (int y = 0; y < 100; ++y)
			{

				// codinatetion
				MathEngine::Vector2 nowCoodinates = { (float)x, (float)y };

				// location
				nowCenter = { center.x + 72.f * (float)y, center.y + 36.f * (float)y };
				MathEngine::Vector2 newLocation = { nowCenter.x, nowCenter.y };

				// 길찾기 데이터에 보관
				Insert(nowCoodinates, nowCenter);
			}
			center = { center.x - 72 , center.y + 36 };
		}
	}

	std::pair<MathEngine::Vector2, MathEngine::Vector2>
		FindPosition::FindNearest(const MathEngine::Vector2 location) 
{
		float minDistance = FLT_MAX;
		std::pair<MathEngine::Vector2, MathEngine::Vector2> closest = 
		{MathEngine::Vector2(0.f, 0.f), MathEngine::Vector2 (0.f, 0.f)};

		for (const auto& d : m_mapData)
		{
			float calDistance = location.Distance(d.second);
			if (calDistance < minDistance)
			{
				minDistance = calDistance;
				closest = d;
			}
		}

		return closest;
	}
}