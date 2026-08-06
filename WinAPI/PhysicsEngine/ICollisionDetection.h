///
/// 충돌 관련 클래스의 인터페이스를 위한 추상 클래스
/// 

#pragma once

class DirectY;

class ICollisionDetection
{
public:
	virtual ~ICollisionDetection() {}

	virtual void CheckCollision() abstract;

	// 디버깅을 위한 충돌범위 그리기
	virtual void Render(DirectY& directY) const abstract;
};