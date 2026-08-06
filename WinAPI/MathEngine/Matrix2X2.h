///
/// 2행 2열 행렬 구조체입니다.
///

#pragma once

#include "Vector2.h"

namespace MathEngine
{
	struct Matrix2X2
	{
		/// 변수 
		// 1행
		float m11;
		float m12;

		// 2행
		float m21;
		float m22;

		// 생성자
		Matrix2X2();
		Matrix2X2(const Matrix2X2& other);
		Matrix2X2(const Vector2& col1, const Vector2& col2);
		Matrix2X2(float m11, float m12, float m21, float m22);

		void Set(const Matrix2X2& other);
		void Set(const Vector2& col1, const Vector2& col2);
		void Set(float m11, float m12, float m21, float m22);

		// 대입 연산자
		Matrix2X2& operator=(const Matrix2X2& rhs);

		// 덧셈
		Matrix2X2 operator+(const Matrix2X2& rhs) const;
		void operator+=(const Matrix2X2& rhs);
		//뺄셈
		Matrix2X2 operator-(const Matrix2X2& rhs) const;
		void operator-=(const Matrix2X2& rhs);

		// 스칼라 곱셈
		Matrix2X2 operator*(const float scalar) const;
		void operator*=(const float scalar);
		// 벡터와 곱셈
		Vector2 operator*(const Vector2& rhs) const;
		// 행렬 곱셈
		Matrix2X2 operator*(const Matrix2X2& rhs) const;
		void operator*=(const Matrix2X2& rhs);

		// 단위행렬로 만드는 함수
		void SetIdentity();

		/// 역행렬
		// 다른 행렬의 역행렬로
		void SetInverseOf(const Matrix2X2& other);
		// 자신 행렬 객체의 역행렬 반환
		Matrix2X2 GetInverse() const;

		// 행렬식
		float GetDeterminant() const;

		/// 전치행렬
		// 다른 행렬의 전치행렬로
		void SetTransposeOf(const Matrix2X2& other);
		// 자신 행렬 객체의 전치행렬 반환
		Matrix2X2 GetTranspose() const;

		// 역행렬과 전치 행렬
		void InvertAndTranspose();

		/// 변환 행렬
		// 회전 행렬
		void Rotation(float angle);
		// 크기 행렬
		void Scale(float scaleX, float scaleY);

		// 디버깅을 위한 출력 함수
		void Show();
	};
}