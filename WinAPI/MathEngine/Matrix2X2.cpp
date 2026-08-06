#include "Matrix2X2.h"

#include "MathUtils.h"
#include <iostream>

namespace MathEngine
{
	Matrix2X2::Matrix2X2()
		: m11(1.0f), m12(0.0f), m21(0.0f), m22(1.0f) // 단위 행렬
	{

	}

	Matrix2X2::Matrix2X2(float m11, float m12, float m21, float m22)
		: m11(m11), m12(m12), m21(m21), m22(m22)
	{

	}

	Matrix2X2::Matrix2X2(const Matrix2X2& other)
		: m11(other.m11), m12(other.m12), m21(other.m21), m22(other.m22)
	{

	}

	Matrix2X2::Matrix2X2(const Vector2& col1, const Vector2& col2)
		: m11(col1.x), m12(col2.x), m21(col1.y), m22(col2.y)
	{

	}

	Matrix2X2& Matrix2X2::operator=(const Matrix2X2& rhs)
	{
		// 일치성 검사
		if (this == &rhs)
		{
			return *this;
		}

		m11 = rhs.m11;
		m12 = rhs.m12;
		m21 = rhs.m21;
		m22 = rhs.m22;

		return *this;
	}

	void Matrix2X2::Set(const Matrix2X2& other)
	{
		m11 = other.m11;
		m12 = other.m12;
		m21 = other.m21;
		m22 = other.m22;
	}

	void Matrix2X2::Set(const Vector2& col1, const Vector2& col2)
	{
		m11 = col1.x;
		m12 = col2.y;
		m21 = col1.y;
		m22 = col2.y;
	}

	void Matrix2X2::Set(float m11, float m12, float m21, float m22)
	{
		this->m11 = m11;
		this->m12 = m12;
		this->m21 = m21;
		this->m22 = m22;
	}

	Matrix2X2 Matrix2X2::operator+(const Matrix2X2& rhs) const
	{
		return Matrix2X2
		(
			m11 + rhs.m11, m12 + rhs.m12,
			m21 + rhs.m21, m22 + rhs.m22
		);
	}

	void Matrix2X2::operator+=(const Matrix2X2& rhs)
	{
		m11 += rhs.m11;
		m12 += rhs.m12;
		m21 += rhs.m21;
		m22 += rhs.m22;
	}

	Matrix2X2 Matrix2X2::operator-(const Matrix2X2& rhs) const
	{
		return Matrix2X2
		(
			m11 - rhs.m11, m12 - rhs.m12,
			m21 - rhs.m21, m22 - rhs.m22
		);
	}

	void Matrix2X2::operator-=(const Matrix2X2& rhs)
	{
		m11 -= rhs.m11;
		m12 -= rhs.m12;
		m21 -= rhs.m21;
		m22 -= rhs.m22;
	}

	Matrix2X2 Matrix2X2::operator*(const float scalar) const
	{
		return Matrix2X2
		(
			scalar * m11, scalar * m12,
			scalar * m21, scalar * m22
		);
	}

	void Matrix2X2::operator*=(const float scalar)
	{
		m11 = scalar;
		m12 = scalar;
		m21 = scalar;
		m22 = scalar;
	}

	Vector2 Matrix2X2::operator*(const Vector2& rhs) const
	{
		return Vector2
		(
			m11 * rhs.x + m21 * rhs.y,
			m12 * rhs.x + m22 * rhs.y
		);
	}


	Matrix2X2 Matrix2X2::operator*(const Matrix2X2& rhs) const
	{
		return Matrix2X2
		(
			m11 * rhs.m11 + m12 * rhs.m21,
			m11 * rhs.m12 + m12 * rhs.m22,
			m21 * rhs.m11 + m22 * rhs.m21,
			m21 * rhs.m12 + m22 * rhs.m22
		);
	}

	void Matrix2X2::operator*=(const Matrix2X2& rhs)
	{
		float x1y1 = m11 * rhs.m11 + m12 * rhs.m21;
		float x1y2 = m11 * rhs.m12 + m12 * rhs.m22;
		float x2y1 = m21 * rhs.m11 + m22 * rhs.m21;
		float x2y2 = m21 * rhs.m12 + m22 * rhs.m22;

		m11 = x1y1;
		m12 = x1y2;
		m21 = x2y1;
		m22 = x2y2;
	}

	void Matrix2X2::SetIdentity()
	{
		m11 = 1.f;
		m12 = 0.f;
		m21 = 0.f;
		m22 = 1.f;
	}

	void Matrix2X2::SetInverseOf(const Matrix2X2& other)
	{
		// 행렬식 ad -bc
		float determinant = other.GetDeterminant();

		// 행렬식 값이 0이면 역행렬이 없는 행렬
		if (determinant == 0.0f)
		{
			return;
		}

		// 역행렬 구하기
		// [d -b]
		// [-c a]
		float x1y1 = other.m22 / determinant;
		float x1y2 = -other.m12 / determinant;
		float x2y1 = -other.m21 / determinant;
		float x2y2 = other.m11 / determinant;

		m11 = x1y1;
		m12 = x1y2;
		m21 = x2y1;
		m22 = x2y2;
	}

	Matrix2X2 Matrix2X2::GetInverse() const
	{
		Matrix2X2 result;
		result.SetInverseOf(*this);
		return result;
	}

	float Matrix2X2::GetDeterminant() const
	{
		return m11 * m22 - m12 * m21;
	}

	void Matrix2X2::SetTransposeOf(const Matrix2X2& other)
	{
		m11 = other.m11;
		m12 = other.m21;
		m21 = other.m12;
		m22 = other.m22;
	}

	Matrix2X2 Matrix2X2::GetTranspose() const
	{
		Matrix2X2 result;
		result.SetTransposeOf(*this);
		return result;
	}

	void Matrix2X2::InvertAndTranspose()
	{
		// 역행렬
		SetInverseOf(*this);
		// 전치행렬
		SetTransposeOf(*this);
	}


	void Matrix2X2::Rotation(float angle)
	{
		angle = MathEngine::DEGREE_TO_RADIAN(angle);

		Matrix2X2 rotation(cos(angle), sin(angle), -sin(angle), cos(angle));

		*this = rotation * (*this);
	}

	void Matrix2X2::Scale(float scaleX, float scaleY)
	{
		m11 = scaleX;
		m12 = 0.f;
		m21 = 0.f;
		m22 = scaleY;
	}

	void Matrix2X2::Show()
	{
		std::cout << "[ " << m11 << ",  " << m12 << " ]" << std::endl;
		std::cout << "[ " << m21 << ",  " << m22 << " ]" << std::endl;
	}
}