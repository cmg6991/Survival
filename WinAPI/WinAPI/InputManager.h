#pragma once
#include <windows.h>
#include "Singleton.h"

#include "../MathEngine/Vector2.h"

enum class KeyState
{
	NONE,
	DOWN,
	PRESS,
	UP
};

class InputManager : public Singleton<InputManager>
{
public:
	void Init(HWND hwnd);
	void Update();

	bool IsGetKeyDown(int key) const { return m_key[key] == KeyState::DOWN; }
	bool IsGetKey(int key) const { return m_key[key] == KeyState::PRESS; }
	bool IsGetKeyUp(int key) const { return m_key[key] == KeyState::UP; }

	bool IsAnyKeyDown() { return m_isKeyDown; }
	bool IsAnyKey() { return m_isKey; }
	bool IsAnyKeyUp() { return m_isKeyUp; }

	MathEngine::Vector2 GetMousePosition();
	void SetMousePosition(float x, float y);

	void SetWheelPower(int power) { m_wheelPower = power; }
	int GetWheelPower() { return m_wheelPower; }

	void ResetWheel() { m_wheelPower = 0; }

private:
	// 한 개의 클래스라는 것을 보장하려면 생성자를 숨겨야 한다.
	InputManager() : m_hwnd(0), m_key{}, m_wheelPower(0), m_isKeyDown(false), m_isKey(0), m_isKeyUp(0) {}
	~InputManager() {}

private:
	HWND m_hwnd;
	int m_wheelPower;

	bool m_isKeyDown;
	bool m_isKey;
	bool m_isKeyUp;

	KeyState m_key[0xFE /*VK_OEM_CLEAR*/ + 1];	// 255, https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes 모든 가상 키에 대응할 수 있게 크기를 정한다.

	MathEngine::Vector2 m_mousePosition{ 0.0f,0.0f };

	friend class Singleton<InputManager>;
};

