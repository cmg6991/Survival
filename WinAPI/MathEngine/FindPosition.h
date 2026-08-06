#pragma once

#include "Vector2.h"

#include <vector>
#include <map>

namespace Algorithm 
{
	class FindPosition
	{
	public:
		std::map<MathEngine::Vector2, MathEngine::Vector2> m_mapData;

		void Insert(
			const MathEngine::Vector2 coordiation, 
			const MathEngine::Vector2 location);

		void Init();

		std::pair<MathEngine::Vector2, MathEngine::Vector2> FindNearest(
			const MathEngine::Vector2 location);
	};
}