#include "pch.h"
#include "Input.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Input* Input::m_pInstance = nullptr;

Input::Input()
	: m_KeyboardDevice(nullptr), m_MouseDevice(nullptr),
	m_Keyboard(), m_Mouse(), m_NowKey(Keyboard::Keys::None), 
	m_KeyboardTracker(nullptr), m_MouseTracker(nullptr),
	m_PrevMousePosX(0), m_PrevMousePosY(0),
	m_MouseDirection(Vector2::Zero), m_MouseDistance(0.f),
	m_isResetCursor(false)
{
	m_KeyboardTracker = new Keyboard::KeyboardStateTracker();
	m_MouseTracker = new Mouse::ButtonStateTracker();
}

Input::~Input()
{

}

void Input::Initalize(HWND window)
{
	if (m_KeyboardDevice == NULL && m_MouseDevice == NULL)
	{
		m_KeyboardDevice = std::make_unique<Keyboard>();
		m_MouseDevice = std::make_unique<Mouse>();
		m_MouseDevice->SetWindow(window);
	}
	
	///m_MouseDevice->SetMode(Mouse::MODE_RELATIVE);
}

void Input::Update()
{
	// 이전 값 저장
	m_PrevMousePosX = m_Mouse.x;
	m_PrevMousePosY = m_Mouse.y;

	if (m_KeyboardDevice->IsConnected())
	{
		m_Keyboard = m_KeyboardDevice->GetState();
		m_KeyboardTracker->Update(m_Keyboard);
	}

	if (m_MouseDevice->IsConnected())
	{
		m_Mouse = m_MouseDevice->GetState();
		m_MouseTracker->Update(m_Mouse);
	}

	SetMouseDirection();
}

// 누르는 도중 계속 호출
bool Input::GetKey(Keyboard::Keys key)
{
	if (m_Keyboard.IsKeyDown(key))
	{
		return true;
	}
	return false;
}

// 눌렀을 때 한번만
bool Input::GetKeyDown(Keyboard::Keys key)
{
	if (m_KeyboardTracker->IsKeyPressed(key))
	{
		return true;
	}
	return false;
}

// 눌렀다가 뗐을 때 한번만
bool Input::GetKeyUp(Keyboard::Keys key)
{
	if (m_KeyboardTracker->IsKeyReleased(key))
	{
		return true;
	}
	return false;
}

bool Input::GetMouseButton(eMouse mouse)
{
	// 누르는 도중 체크.
	return false;
}

bool Input::GetMouseButtonDown(eMouse mouse)
{
	bool _state = false;

	switch (mouse)
	{
		case Input::eMouse::leftButton:
			if (m_MouseTracker->leftButton == Mouse::ButtonStateTracker::ButtonState::PRESSED)
			{
				_state = true;
			}
			break;
		case Input::eMouse::middleButton:
			if (m_MouseTracker->middleButton == Mouse::ButtonStateTracker::ButtonState::PRESSED)
			{
				_state = true;
			}
			break;
		case Input::eMouse::rightButton:
			if (m_MouseTracker->rightButton == Mouse::ButtonStateTracker::ButtonState::PRESSED)
			{
				_state = true;
			}
			break;
		case Input::eMouse::xButton1:
			if (m_MouseTracker->xButton1 == Mouse::ButtonStateTracker::ButtonState::PRESSED)
			{
				_state = true;
			}
			break;
		case Input::eMouse::xButton2:
			if (m_MouseTracker->xButton2 == Mouse::ButtonStateTracker::ButtonState::PRESSED)
			{
				_state = true;
			}
			break;
		default:
			break;
	}

	return _state;
}

bool Input::GetMouseButtonUp(eMouse mouse)
{
	bool _state = false;

	switch (mouse)
	{
		case Input::eMouse::leftButton:
			if (m_MouseTracker->leftButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
			{
				_state = true;
			}
			break;
		case Input::eMouse::middleButton:
			if (m_MouseTracker->middleButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
			{
				_state = true;
			}
			break;
		case Input::eMouse::rightButton:
			if (m_MouseTracker->rightButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
			{
				_state = true;
			}
			break;
		case Input::eMouse::xButton1:
			if (m_MouseTracker->xButton1 == Mouse::ButtonStateTracker::ButtonState::RELEASED)
			{
				_state = true;
			}
			break;
		case Input::eMouse::xButton2:
			if (m_MouseTracker->xButton2 == Mouse::ButtonStateTracker::ButtonState::RELEASED)
			{
				_state = true;
			}
			break;
		default:
			break;
	}

	return _state;
}

DirectX::SimpleMath::Vector2 Input::GetMousePos()
{
	return DirectX::SimpleMath::Vector2(m_Mouse.x, m_Mouse.y);
}

DirectX::SimpleMath::Vector2 Input::GetPrevMousePos()
{
	return DirectX::SimpleMath::Vector2(m_PrevMousePosX, m_PrevMousePosY);
}

void Input::SetMouseDirection()
{
	// 도착점 - 시작점
	m_MouseDirection = Vector2(m_Mouse.x, m_Mouse.y) - Vector2(m_PrevMousePosX, m_PrevMousePosY);
	m_MouseDirection.Normalize();
	m_MouseDistance = Vector2::Distance(Vector2(m_Mouse.x, m_Mouse.y), Vector2(m_PrevMousePosX, m_PrevMousePosY));
}

int Input::GetMouseScrollValue()
{
	int a =	m_MouseDevice->GetState().scrollWheelValue;
	//m_MouseDevice->ResetScrollWheelValue();

	return a;
}

DirectX::SimpleMath::Vector2 Input::GetMouseDIstanceVec2()
{
	Vector2 dis;
	dis.x = m_Mouse.x - m_PrevMousePosX;
	dis.y = m_Mouse.y - m_PrevMousePosY;
	return dis;
}

