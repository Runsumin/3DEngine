#pragma once
#include "Gamepad.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "3DGraphicsEngine.h"

class Input
{
public:
	enum class eMouse
	{
		leftButton,		// 왼쪽 버튼
		middleButton,	// 휠 버튼
		rightButton,	// 오른쪽 버튼
		xButton1,		// 사이드 위 버튼
		xButton2,		// 사이드 아래 버튼
	};

private:
	// Input devices.
	std::unique_ptr<DirectX::Keyboard>			m_KeyboardDevice;
	std::unique_ptr<DirectX::Mouse>				m_MouseDevice;

private:
	DirectX::Keyboard::State					m_Keyboard;
	DirectX::Mouse::State						m_Mouse;
	DirectX::Keyboard::Keys						m_NowKey;

	DirectX::Keyboard::KeyboardStateTracker*	m_KeyboardTracker;
	DirectX::Mouse::ButtonStateTracker*			m_MouseTracker;

	int m_PrevMousePosX;
	int m_PrevMousePosY;

	DirectX::SimpleMath::Vector2 m_MouseDirection;
	float m_MouseDistance;

	bool m_isResetCursor;

#pragma region SINGLETON
private:
	static Input* m_pInstance;

public:
	static Input* Ins()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new Input();
		}
		return m_pInstance;
	}
#pragma endregion

private:
	Input();
	~Input();

public:
	void Initalize(HWND window);
	void Update();
	bool GetKey(DirectX::Keyboard::Keys key);
	bool GetKeyDown(DirectX::Keyboard::Keys key);
	bool GetKeyUp(DirectX::Keyboard::Keys key);
	bool GetMouseButton(eMouse mouse);
	bool GetMouseButtonDown(eMouse mouse);
	bool GetMouseButtonUp(eMouse mouse);

	DirectX::SimpleMath::Vector2 GetMousePos();
	DirectX::SimpleMath::Vector2 GetPrevMousePos();
	DirectX::SimpleMath::Vector2 GetMouseDirection() const { return m_MouseDirection; }
	float GetMouseDistance() const { return m_MouseDistance; }

	void SetMouseDirection();

	void SetMouseLock(bool state) { m_isResetCursor = state; }
	int GetMouseScrollValue();

	DirectX::SimpleMath::Vector2 GetMouseDIstanceVec2();
};

