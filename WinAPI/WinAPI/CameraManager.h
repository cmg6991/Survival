#pragma once
#include "Singleton.h"

class Transform;

//ΩÃ±€≈Ê¿∏∑Œ ∏∏µÈ±‚
class CameraManager : public Singleton<CameraManager>
{
public:
	void Init();

	void Follow(Transform* transform);

	//void Translate(Vector2 position);
	float GetX() const { return m_x; }
	float GetY() const { return m_y; }

private:
	float m_x;
	float m_y;

private:
	CameraManager() : m_x(0), m_y(0){}
	~CameraManager(){}

	friend class Singleton<CameraManager>;
};

