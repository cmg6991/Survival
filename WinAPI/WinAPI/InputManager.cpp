#include "InputManager.h"
#include <stdio.h>
void InputManager::Init(HWND hwnd)
{
	m_hwnd = hwnd;
	for (int i = 0; i < 255; i++)
	{
		m_key[i] = KeyState::NONE;
	}
}

void InputManager::Update()
{
	m_isKeyDown = false;
	m_isKey = false;
	m_isKeyUp = false;

	for (int i = 0; i < 255; i++)
	{
		/// 이번 프레임에 키 누름
		// 이전 프레임에 None or Up 이었는데 눌렸다. -> Down
		if ((m_key[i] == KeyState::NONE || m_key[i] == KeyState::UP)
			&& GetAsyncKeyState(i) & 0x8000)
		{
			m_key[i] = KeyState::DOWN;
			m_isKeyDown = true;
		}
		// 이전 프레임에 Down 이었는데 눌렸다. -> Press
		else if (m_key[i] == KeyState::DOWN && GetAsyncKeyState(i) & 0x8000)
		{
			m_key[i] = KeyState::PRESS;
			m_isKey = true;
		}

		/// 이번 프레임에 키 안 누름
		// 이전 프레임에 Down, Press 이었는데 안눌렀다. -> Up
		else if ((m_key[i] == KeyState::DOWN || m_key[i] == KeyState::PRESS)
			&& !(GetAsyncKeyState(i) & 0x8000))
		{
			m_key[i] = KeyState::UP;
			m_isKeyUp = true;
		}
		// 이전 프레임에 Up 이었는데 안눌렀다.
		else if (m_key[i] == KeyState::UP && !(GetAsyncKeyState(i) & 0x8000))
		{
			m_key[i] = KeyState::NONE;
		}
	}
}

MathEngine::Vector2 InputManager::GetMousePosition()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(m_hwnd, &mousePos);


	POINT screenPos = mousePos;

	return { (float)mousePos.x, (float)mousePos.y };
	//return m_mousePosition;
}

void InputManager::SetMousePosition(float x, float y)
{
	m_mousePosition.x = x;
	m_mousePosition.y = y;
}
