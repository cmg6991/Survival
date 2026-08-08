#include "TimeManager.h"
#include "pch.h"

void TimeManager::Init()
{
    m_timer = 0.0f;

    // 시작 시간
    m_day = 1;
    m_hour = 6;
    m_minute = 0;

    // 테스트용
    // 현실 1초 = 게임 10분

    //m_timeScale = 10.0f;
    m_gameMinuteTimer = 0.0f;

    m_currentPhase = GetTimePhase();
    m_previousPhase = m_currentPhase;
}

void TimeManager::Update(float deltaTime)
{
    m_gameMinuteTimer += deltaTime;

    if (m_gameMinuteTimer >=1.0f)
    {
        m_gameMinuteTimer -= 1.0f;
        // 게임 시간 10분 증가
        m_minute += 10;
    }

    // 60분 -> 1시간

    if (m_minute >= 60)
    {
        m_minute -=60;

        m_hour++;
    }

    // 24시간 -> 다음날

    if (m_hour >= 24)
    {
        m_hour = 0;

        m_day++;
    }
    TimePhase newPhase = GetTimePhase();


    if (newPhase != m_previousPhase)
    {
        OnTimePhaseChanged(newPhase);

        m_previousPhase = newPhase;
    }
}

bool TimeManager::IsDay() const
{
    // 06:00 ~ 17:59

    if (m_hour >= 6 &&
        m_hour < 18)
    {
        return true;
    }


    return false;
}

bool TimeManager::IsNight() const
{
    return !IsDay();
}

TimePhase TimeManager::GetTimePhase() const
{
    if (m_hour >= 6 && m_hour < 12)
    {
        return TimePhase::Morning;
    }


    if (m_hour >= 12 && m_hour < 18)
    {
        return TimePhase::Day;
    }


    if (m_hour >= 18 && m_hour < 24)
    {
        return TimePhase::Evening;
    }


    return TimePhase::Night;
}

void TimeManager::OnTimePhaseChanged(TimePhase phase)
{
    switch (phase)
    {
    case TimePhase::Morning:
        OutputDebugString(L"아침 시작\n");
        break;


    case TimePhase::Day:
        OutputDebugString(L"낮 시작\n");
        break;


    case TimePhase::Evening:
        OutputDebugString(L"저녁 시작\n");
        break;


    case TimePhase::Night:
        OutputDebugString(L"밤 시작\n");
        break;

    }
}

const wchar_t* TimeManager::GetPhaseString() const
{
    switch (GetTimePhase())
    {

    case TimePhase::Morning:
        return L"아침";

    case TimePhase::Day:
        return L"낮";

    case TimePhase::Evening:
        return L"저녁";

    case TimePhase::Night:
        return L"밤";
    }

    return L"";
}

void TimeManager::SetTime(int day, int hour, int minute)
{
    m_day = day;
    m_hour = hour;
    m_minute = minute;

    m_gameMinuteTimer = 0.0f;

    m_currentPhase = GetTimePhase();
    m_previousPhase = m_currentPhase;
}
