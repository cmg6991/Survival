#pragma once

#include "pch.h"
#include "Singleton.h"
#include "../MathEngine/Vector2.h"

class CampFire;
class Player;

class EnvironmentManager:public Singleton<EnvironmentManager>
{
public:
	void Init(Player* player);
	void Update(float deltaTime);
	void Render(ID2D1DeviceContext* context);

	void RegisterCampFire(CampFire* fire);
	void UnregisterCampFire(CampFire* fire);

	void CreateFireLight(ID2D1DeviceContext* context);

	//void SetFirePosition(const MathEngine::Vector2& position) { m_firePosition = position; }*/

	void CreateRingLight(ID2D1DeviceContext* context);
	void RenderRingLight(ID2D1DeviceContext* context);
private:
	Player* m_player = nullptr;
	float GetNightAlpha();

	float m_nightAlpha = 0.0f;

	ID2D1SolidColorBrush* m_nightBrush = nullptr;
	ID2D1RadialGradientBrush* m_fireLightBrush = nullptr;
	ID2D1RadialGradientBrush* m_ringLightBrush = nullptr;

	const float m_fireLightRadius = 250.0f;

	float m_ringLightRadius = 180.0f;
	std::vector<CampFire*> m_campFires;
private:
	EnvironmentManager() {}
	~EnvironmentManager() {}

	friend class Singleton<EnvironmentManager>;
};

