#pragma once
#include "Singleton.h"

enum class TimePhase
{
	Morning,
	Day,
	Evening,
	Night
};

class TimeManager : public Singleton<TimeManager>
{
public:
	void Init();
	void Update(float deltaTime);

	int GetDay() const { return m_day; }
	int GetHour() const { return m_hour; }
	int GetMinute() const { return m_minute; }
	
	bool IsDay() const;
	bool IsNight() const;

	void SetTimeScale(float scale) { m_timeScale = scale; }
	float GetTimeScale() const { return m_timeScale; }

	TimePhase GetTimePhase() const;
	void OnTimePhaseChanged(TimePhase phase);

	const wchar_t* GetPhaseString() const;
	void SetTime(int day, int hour, int minute);

private:
	TimeManager() : m_timer(0.f), m_day(0), m_hour(0), m_minute(0), m_timeScale(0.f), m_gameMinuteTimer(0) {};
	~TimeManager() {};
	
private:
	float m_timer;

	int m_day;
	int m_hour;
	int m_minute;

	float m_timeScale;
	float m_gameMinuteTimer;

	TimePhase m_currentPhase;
	TimePhase m_previousPhase;

	friend class Singleton<TimeManager>;
};

