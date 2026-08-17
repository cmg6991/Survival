#pragma once
#include "pch.h"
#include "../MathEngine/Vector2.h"

class TileMap;
class Player;
class Inventory;

enum class FishingState
{
	Idle,
	Waiting,
	Ready
};

class FishingController
{
public:
    void Init(TileMap* tileMap, Player* player);
    void Update(float deltaTime);
    bool TryStartFishing(const MathEngine::Vector2& playerPos);
    bool IsFishing() const { return m_state != FishingState::Idle; }

private:
    bool IsNearWater(const MathEngine::Vector2& pos) const;

private:
    TileMap* m_tileMap = nullptr;
    Player* m_player = nullptr;
    FishingState m_state = FishingState::Idle;
    float m_waitTimer = 0.0f;
};

