#pragma once
#include "pch.h"

#include "../MathEngine/Vector2.h"

class CollisionManager;
class GameObject;

class MonsterSpawner
{
public:
	MonsterSpawner();
	~MonsterSpawner();

    void Init(CollisionManager* collisionManager, int mapWidth, int mapHeight);
    void Update(float deltaTime, const MathEngine::Vector2& playerPos);

    //// MainScene이 실제 생성 로직을 넘겨줌 (Monster 생성은 여전히 MainScene 책임)
    //void SetOnSpawnRequest(std::function<void(float x, float y)> callback) { m_onSpawnRequest = callback; }
    //// 아침이 되면 살아있는 몬스터 전부 제거를 요청
    void SetOnClearAllMonsters(std::function<void()> callback) { m_onClearAllMonsters = callback; }

    void SetOnSpawnRequest(std::function<void(const string&, float, float)> callback) { m_onSpawnRequest = callback;}

    void SetSpawnPool(const std::vector<string>& monsterIds) {m_spawnPoolIds = monsterIds;}

    bool IsNightActive() const { return m_isNightSpawnActive; }

private:
    bool IsNightTime() const;
    int GetNightHourIndex() const;
    MathEngine::Vector2 GetRandomSpawnPositionOffscreen(const MathEngine::Vector2& playerPos) const;
    void SpawnAroundPlayer(int count, const MathEngine::Vector2& playerPos);
    string ChooseMonsterId(int dayFactor) const;

private:
    CollisionManager* m_collisionManager = nullptr;
    //std::function<void(float, float)> m_onSpawnRequest;
    std::function<void()> m_onClearAllMonsters;

    int m_mapWidth = 100;
    int m_mapHeight = 100;

    bool m_isNightSpawnActive = false;
    float m_spawnTimer = 0.0f;
    float m_nightElapsedTime = 0.0f;
    int m_currentDay = 1;
    float m_spawnRadius = 15.0f;

    std::function<void(const string&, float, float)> m_onSpawnRequest;
    std::vector<string> m_spawnPoolIds;
};

