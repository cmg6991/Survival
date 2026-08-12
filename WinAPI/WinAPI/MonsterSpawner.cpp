#include "MonsterSpawner.h"
#include "pch.h"
#include "CollisionManager.h"
#include "TimeManager.h"

MonsterSpawner::MonsterSpawner()
{
}

MonsterSpawner::~MonsterSpawner()
{
}

void MonsterSpawner::Init(CollisionManager* collisionManager, int mapWidth, int mapHeight)
{
	m_collisionManager = collisionManager;
	m_mapWidth = mapWidth;
	m_mapHeight = mapHeight;
}

void MonsterSpawner::Update(float deltaTime, const MathEngine::Vector2& playerPos)
{
    m_currentDay = TimeManager::GetInstance().GetDay();
    bool nightNow = IsNightTime();

    if (nightNow && !m_isNightSpawnActive)
    {
        m_isNightSpawnActive = true;
        //m_nightElapsedTime = 0.0f;
        m_spawnTimer = 0.0f;
    }
    else if (!nightNow && m_isNightSpawnActive)
    {
        m_isNightSpawnActive = false;
        if (m_onClearAllMonsters)
            m_onClearAllMonsters();   // 아침이 되면 전부 제거
    }

    if (!m_isNightSpawnActive)
        return;

    //m_nightElapsedTime += deltaTime;
    m_spawnTimer -= deltaTime;
    if (m_spawnTimer > 0.0f) return;

    int dayFactor = max(0, m_currentDay - 1);
    int nightHourIndex = GetNightHourIndex();

    float baseInterval = max(0.6f, 3.0f - dayFactor * 0.2f);
    float spawnInterval = max(0.3f, baseInterval - nightHourIndex * 0.15f);

    // 스폰 개수: 날짜 + 밤 진행도 둘 다 반영
    int baseCount = 1 + dayFactor;
    int spawnCount = baseCount + (nightHourIndex / 3);

    SpawnAroundPlayer(spawnCount, playerPos);
    m_spawnTimer = spawnInterval;
}

bool MonsterSpawner::IsNightTime() const
{
	int hour = TimeManager::GetInstance().GetHour();
	return (hour >= 18 || hour < 6);
}

int MonsterSpawner::GetNightHourIndex() const
{
    int hour = TimeManager::GetInstance().GetHour();
    if (hour >= 18)
        return hour - 18;
    else
        return hour + 6;
}

MathEngine::Vector2 MonsterSpawner::GetRandomSpawnPositionOffscreen(const MathEngine::Vector2& playerPos) const
{
    float angle = ((float)rand() / RAND_MAX) * 2.0f * 3.14159265f;

    MathEngine::Vector2 offset;
    offset.x = cosf(angle) * m_spawnRadius;
    offset.y = sinf(angle) * m_spawnRadius;

    MathEngine::Vector2 spawnPos = playerPos + offset;

    spawnPos.x = max(1.0f, min(spawnPos.x, (float)(m_mapWidth - 2)));
    spawnPos.y = max(1.0f, min(spawnPos.y, (float)(m_mapHeight - 2)));

    return spawnPos;
}

void MonsterSpawner::SpawnAroundPlayer(int count, const MathEngine::Vector2& playerPos)
{
    if (!m_onSpawnRequest) return;

    int dayFactor = max(0, m_currentDay - 1);

    for (int i = 0; i < count; i++)
    {
        MathEngine::Vector2 pos = GetRandomSpawnPositionOffscreen(playerPos);

        if (m_collisionManager != nullptr &&
            m_collisionManager->IsBlocked((int)round(pos.x), (int)round(pos.y)))
            continue;

        m_onSpawnRequest(ChooseMonsterId(dayFactor), pos.x, pos.y);
    }
}

string MonsterSpawner::ChooseMonsterId(int dayFactor) const
{
    int weightSmall = max(1, 10 - dayFactor * 2);   // 날짜 지날수록 감소
    int weightNormal = 5 + dayFactor;                     // 서서히 증가
    int weightBig = max(0, dayFactor - 1);            // 3일차부터 등장 시작, 계속 증가

    int total = weightSmall + weightNormal + weightBig;
    int roll = rand() % total;

    if (roll < weightSmall) return "Monster2";
    roll -= weightSmall;
    if (roll < weightNormal) return "Monster";
    return "Monster3";
}
