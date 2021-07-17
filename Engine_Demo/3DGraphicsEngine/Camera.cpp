#include "Camera.h"

using namespace GRAPHICENGINE;

Camera::Camera(Vector3 pos, Vector3 rot)
	: m_PosBk(Vector3::Zero), m_RoatationBk(Vector3::Zero), m_ScaleBk(Vector3::Zero),
	m_PosRtBk(Vector3::Zero), m_RoatationRtBk(Vector3::Zero), m_ScaleRtBk(Vector3::Zero),
	m_Fov(0.f), m_Near(0.f), m_AspectRatio(0.f), m_Far(0.f), m_CameraSpeed(0.f), m_CameraMode(ICameraInterface::Mode::DEFAULT)
	, m_Rotation(Vector3::Zero)
{
	///
	m_Position = pos;
	m_Rotation = rot;
	m_Scale = Vector3::One;
	///

	m_PosBk = Vector3::Zero;
	m_RoatationBk = Vector3::Zero;
	m_ScaleBk = Vector3::Zero;

	m_View = XMMatrixIdentity();
	m_Proj = XMMatrixIdentity();

	ViewW = 1920;
	ViewH = 1080;

}

Camera::~Camera()
{

}

void Camera::InitializeCamera()
{
	//m_Position = Vector3(85, 115, 93);
	//m_Rotation = Vector3(0.5f, -2.345f, 0);
	//m_Scale = Vector3::One;

	m_PosBk = m_Position;
	m_RoatationBk = m_Rotation;
	m_ScaleBk = m_Scale;

	m_View = Matrix::CreateScale(m_Scale) * XMMatrixRotationRollPitchYawFromVector(m_Rotation) * Matrix::CreateTranslation(m_Position);
	m_Fov = 60;
	m_AspectRatio = D3DDevice::GetInstance()->m_ScreenWidth / D3DDevice::GetInstance()->m_ScreenHeight;
	m_Near = 1.0f;
	m_Far = 300.f;

	m_CameraSpeed = 3000;
	m_CameraMode = ICameraInterface::Mode::DEFAULT;

}

void Camera::ResetCamera()
{
	m_Position = m_PosBk;
	m_Rotation = m_RoatationBk;
	m_Scale = m_ScaleBk;
	m_CameraMode = ICameraInterface::Mode::DEFAULT;
	m_Rotation = Vector3::Zero;
}

void Camera::SetCamerSRTData(Vector3 pos, Vector3 rot, Vector3 scl)
{
	m_Position = pos;
	m_Rotation = rot;
	m_Scale = scl;
}

void Camera::ChangeCameraMode(ICameraInterface::Mode ecameramode)
{
	switch (ecameramode)
	{
	case ICameraInterface::Mode::DEFAULT:
		m_CameraMode = ICameraInterface::Mode::DEFAULT;
		break;
	case ICameraInterface::Mode::MOVE:
		m_CameraMode = ICameraInterface::Mode::MOVE;
		break;
	case ICameraInterface::Mode::AUTOROTATION:
		m_CameraMode = ICameraInterface::Mode::AUTOROTATION;
		break;
	case ICameraInterface::Mode::ORTHO:
		m_CameraMode = ICameraInterface::Mode::ORTHO;
		break;
	case ICameraInterface::Mode::INPUT:
		m_CameraMode = ICameraInterface::Mode::INPUT;
		break;
	}
}

void Camera::SetCameraSpeed(float speed)
{
	m_CameraSpeed = speed;
}

void Camera::RestoreCamera()
{
	m_Position = m_PosRtBk;
	m_Rotation = m_RoatationRtBk;
	m_Scale = m_ScaleRtBk;
}

void Camera::BackUpCameraData()
{
	m_PosRtBk = m_Position;
	m_RoatationRtBk = m_Rotation;
	m_ScaleRtBk = m_Scale;
}

void Camera::UpdateCamera(float dTime)
{
	float degree;
	switch (m_CameraMode)
	{
	case ICameraInterface::Mode::DEFAULT:
		// 초기 상태를 따라간다
		m_View = Matrix::CreateScale(m_Scale) * XMMatrixRotationRollPitchYawFromVector(m_Rotation) * Matrix::CreateTranslation(m_Position);
		break;

	case ICameraInterface::Mode::AUTOROTATION:

		m_Position += Vector3::TransformNormal(Vector3::Left, XMMatrixRotationRollPitchYawFromVector(m_Rotation)) * dTime * m_CameraSpeed;
		m_Rotation.y += XM_PIDIV4 * dTime;
		m_View = Matrix::CreateScale(m_Scale) * XMMatrixRotationRollPitchYawFromVector(m_Rotation) * Matrix::CreateTranslation(m_Position);
		break;

	case ICameraInterface::Mode::MOVE:
		degree = m_Rotation.y * 180 / XM_PI;
		if (degree < 0.0f)
		{
			degree += 360.f;
		}
		if (degree > 360.f)
		{
			degree -= 360.f;
		}

		m_Rotation.y = degree / 180 * XM_PI;

		// 이동 카메라
		if (GetAsyncKeyState('W') & 0x8000)
		{
			m_Position += Vector3::TransformNormal(Vector3::UnitZ, XMMatrixRotationRollPitchYawFromVector(m_Rotation)) * dTime * m_CameraSpeed;
		}
		if (GetAsyncKeyState('S') & 0x8000)
		{
			m_Position -= Vector3::TransformNormal(Vector3::UnitZ, XMMatrixRotationRollPitchYawFromVector(m_Rotation)) * dTime * m_CameraSpeed;
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			m_Position += Vector3::TransformNormal(Vector3::Left, XMMatrixRotationRollPitchYawFromVector(m_Rotation)) * dTime * m_CameraSpeed;
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			m_Position += Vector3::TransformNormal(Vector3::Right, XMMatrixRotationRollPitchYawFromVector(m_Rotation)) * dTime * m_CameraSpeed;
		}
		if (GetAsyncKeyState('Q') & 0x8000)
		{
			m_Position += Vector3::TransformNormal(Vector3::UnitY, XMMatrixRotationRollPitchYawFromVector(m_Rotation)) * dTime * m_CameraSpeed;
		}
		if (GetAsyncKeyState('E') & 0x8000)
		{
			m_Position -= Vector3::TransformNormal(Vector3::UnitY, XMMatrixRotationRollPitchYawFromVector(m_Rotation)) * dTime * m_CameraSpeed;
		}

		//rotation
		if (GetAsyncKeyState(VK_HOME) & 0x8000)
		{
			m_Rotation.x -= XM_PIDIV2 * dTime;
		}
		if (GetAsyncKeyState(VK_END) & 0x8000)
		{
			m_Rotation.x += XM_PIDIV2 * dTime;
		}
		if (GetAsyncKeyState(VK_DELETE) & 0x8000)
		{
			m_Rotation.y -= XM_PIDIV2 * dTime;
		}
		if (GetAsyncKeyState(VK_NEXT) & 0x8000)
		{
			m_Rotation.y += XM_PIDIV2 * dTime;
		}
		m_View = Matrix::CreateScale(m_Scale) * XMMatrixRotationRollPitchYawFromVector(m_Rotation) * Matrix::CreateTranslation(m_Position);
		break;
	case ICameraInterface::Mode::FOLLOW:
		/*	m_Rotation = Vector3(0.6, -0.69, 0);
			m_Position = Vector3(550, 720, -623);
			m_View = Matrix::CreateScale(m_Scale) * XMMatrixRotationRollPitchYawFromVector(m_Rotation) *
				Matrix::CreateTranslation(m_Position) * Matrix::CreateTranslation(playerpos);*/
		break;
	case ICameraInterface::Mode::ORTHO:
		m_View = Matrix::CreateScale(m_Scale) * XMMatrixRotationRollPitchYawFromVector(m_Rotation) * Matrix::CreateTranslation(m_Position);
		break;
	case ICameraInterface::Mode::INPUT:
		degree = m_Rotation.y * 180 / XM_PI;
		if (degree < 0.0f)
		{
			degree += 360.f;
		}
		if (degree > 360.f)
		{
			degree -= 360.f;
		}

		m_Rotation.y = degree / 180 * XM_PI;

		m_View = Matrix::CreateScale(m_Scale) * XMMatrixRotationRollPitchYawFromVector(m_Rotation) * Matrix::CreateTranslation(m_Position);
		break;
	}
	// 마지막 매트릭스 만들어주기.
	SetCameraMatrixData();
}

Matrix Camera::GetCameraViewMat()
{
	return m_View;
}

Matrix Camera::GetCameraProjMat()
{
	return m_Proj;
}

DirectX::SimpleMath::Vector3 Camera::GetCameraPos()
{
	return m_Position;
}

DirectX::SimpleMath::Vector3 Camera::GetCameraRot()
{
	return m_Rotation;
}

DirectX::SimpleMath::Vector3 Camera::GetCameraScl()
{
	return m_Scale;
}

void Camera::SetCameraMatrixData()
{
	// 행렬값 만들기
	m_View = m_View.Invert();
	switch (m_CameraMode)
	{
	case ICameraInterface::Mode::DEFAULT:
		m_Proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_Fov), m_AspectRatio, m_Near, m_Far);
		break;
	case ICameraInterface::Mode::AUTOROTATION:
		m_Proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_Fov), m_AspectRatio, m_Near, m_Far);
		break;
	case ICameraInterface::Mode::MOVE:
		m_Proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_Fov), m_AspectRatio, m_Near, m_Far);
		break;
	case ICameraInterface::Mode::FOLLOW:
		m_Proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_Fov), m_AspectRatio, m_Near, m_Far);
		break;
	case ICameraInterface::Mode::ORTHO:
		m_Proj = XMMatrixOrthographicLH(ViewW, ViewH, m_Near, m_Far);
		break;
	case ICameraInterface::Mode::INPUT:
		m_Proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_Fov), m_AspectRatio, m_Near, m_Far);
		break;
	default:
		break;
	}

}

void Camera::GetCameraData_FromGameEngine(Matrix view, Matrix proj)
{

}

void Camera::CameraInfo(int width, int height)
{
	va_list tmp = nullptr;

	DrawText_Debug::GetInstance()->DrawTextExW(width, 15, XMFLOAT4(255, 255, 255, 1),100.0f, L"[카메라 이동] : WSADQE", tmp);
	DrawText_Debug::GetInstance()->DrawTextExW(width, 30, XMFLOAT4(255, 255, 255, 1),100.0f, L"[카메라 회전] : HOME END DEL PGDN", tmp);
	DrawText_Debug::GetInstance()->DrawTextExW(width, 45, XMFLOAT4(255, 255, 255, 1),100.0f, L"Render Target View : F9", tmp);
	DrawText_Debug::GetInstance()->DrawTextExW(width, 60, XMFLOAT4(255, 255, 255, 1), 100.0f, L"WireFrame: SPACEBAR", tmp);

	DrawText_Debug::GetInstance()->DrawTextExW(1700, 5, XMFLOAT4(255, 255, 255, 1), 100.0f, L"NUM_1: FINAL VIEW", tmp);
	DrawText_Debug::GetInstance()->DrawTextExW(1700, 20, XMFLOAT4(255, 255, 255, 1), 100.0f, L"NUM_2: Diffuse", tmp);
	DrawText_Debug::GetInstance()->DrawTextExW(1700, 35, XMFLOAT4(255, 255, 255, 1), 100.0f, L"NUM_3: Normal", tmp);
	DrawText_Debug::GetInstance()->DrawTextExW(1700, 50, XMFLOAT4(255, 255, 255, 1), 100.0f, L"NUM_4: Ambient Occlusion", tmp);
	DrawText_Debug::GetInstance()->DrawTextExW(1700, 65, XMFLOAT4(255, 255, 255, 1), 100.0f, L"NUM_5: Metallic", tmp);
	DrawText_Debug::GetInstance()->DrawTextExW(1700, 80, XMFLOAT4(255, 255, 255, 1), 100.0f, L"NUM_6: Roughness", tmp);
	DrawText_Debug::GetInstance()->DrawTextExW(1700, 95, XMFLOAT4(255, 255, 255, 1), 100.0f, L"NUM_7: IBL", tmp);
	DrawText_Debug::GetInstance()->DrawTextExW(1700, 110, XMFLOAT4(255, 255, 255, 1), 100.0f, L"NUM_8: PBR Texture", tmp);
}

void Camera::CameraInfo()
{
	CameraInfo(0, 0);
}

void Camera::ReleaseCamera()
{

}
