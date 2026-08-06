///
/// 필요한 수학 관련 함수들이 인라인 함수로 래핑(wrapping) 되어 있습니다.
/// 

#pragma once

#include <cmath>
#include "Constants.h"

namespace MathEngine
{
	// 두 변수의 자리를 바꾸는 함수
	inline void Swap(float& lhs, float& rhs)
	{
		const float temp = lhs;
		lhs = rhs;
		rhs = temp;
	}
	
	/// <summary>
	/// 부동 소수값을 비교하는 데 사용: 
	/// 컴퓨터의 부동 소수점은 오차로 인해 비교가 어렵기 때문에
	/// 두 실수 값이 거이 같은지를 판단하는 데 사용합니다.
	/// </summary>
	/// <param name="lhs">비교할 flaot 값 1</param>
	/// <param name="rhs">비교할 float 값 2</param>
	/// <param name="epsilon">오차 허용 범위</param>
	inline bool fuzzyRealEqual(const float& lhs, const float& rhs, const float& epsilon = Constant::FLOAT_TOLERANCE)
	{
		return std::fabs(lhs - rhs) < epsilon;
	}

	/// <summary>
	/// 부동 소수값을 비교
	/// </summary>
	/// <param name="lhs">비교할 flaot 값 1</param>
	/// <param name="rhs">비교할 float 값 2</param>
	inline bool IsEqual(const float& lhs, const float& rhs)
	{
		return fuzzyRealEqual(lhs, rhs);
	}

	/// 호도법
	// 도(°)에서 라디안(rad)으로
	inline float DEGREE_TO_RADIAN(const float angle)
	{
		return angle * Constant::PI / 180.f;
	}
	// 라디안(rad)에서 도(°)로
	inline float RADIAN_TO_DEGREE(const float angle)
	{
		return angle * 180.f / Constant::PI;
	}
}