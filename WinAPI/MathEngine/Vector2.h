///
/// 2차원 벡터 구조체 입니다.
/// 

#pragma once

namespace MathEngine
{
	struct Vector2
	{
		// 변수
		float x;
		float y;

		// 생성자
		Vector2(float x = 0.0f, float y = 0.0f);
		// 복사 생성자
		Vector2(const Vector2& copy);

		void set(float x, float y);

		// 대입
		Vector2& operator=(const Vector2& rhs);

		// 논리 연산
		bool operator==(const Vector2& rhs) const;
		bool operator!=(const Vector2& rhs) const;

		// 덧셈
		Vector2 operator+(const Vector2& rhs) const;
		void operator+=(const Vector2& rhs);

		// 뺄셈
		Vector2 operator-(const Vector2& rhs) const;
		void operator-=(const Vector2& rhs);

		// 스칼라 곱샘
		Vector2 operator*(const float scalar) const;
		void operator*=(const float scala);

		// 스칼라 나눗셈
		Vector2 operator/(const float scala) const;
		void operator/=(const float scala);

		// 맵 사용을 위한 비교연산자
		bool operator<(const Vector2& other) const;

		/// 내적(점곱)
		// * 연산자를 통한 내적 계산
		float operator*(const Vector2& rhs) const;
		// 명시적으로 나타낸 내적 함수
		float Dot(const Vector2& other) const;

		// 외적
		float Cross(const Vector2& other) const;

		// 두 벡터의 사이 각을 구하는 함수
		float Angle(const Vector2& other) const;

		Vector2 Rotaie(const float& raian) const;

		// 두 벡터의 거리 계산
		float Distance(const Vector2& other) const;

		// 표준화
		Vector2 Normalize() const;

		// 벡터의 크기(길이)
		float Magnitude() const;
		
		// x, y 를 절대값으로 만드는 함수
		void Absolute();

		// x, y 를 0으로 만드는 함수
		void Zero();

		// 디버깅을 위한 출력 함수
		void Show() const;

	};
}