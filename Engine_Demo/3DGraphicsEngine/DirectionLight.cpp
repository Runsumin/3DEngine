#include "DirectionLight.h"

DirectionLight::DirectionLight(Vector3 pos, Vector4 direction, Vector4 diffuse, Vector4 ambient, float range)
{
	m_Direction = direction;
	m_Diffuse = diffuse;
	m_Ambient = ambient;
	m_LitPos = pos;
	m_Range = range;
}

DirectionLight::~DirectionLight()
{

}

void DirectionLight::InitializeLight()
{
	/// 상수버퍼 정보 초기화
	ZeroMemory(&m_CBLight, sizeof(CBLIGHT));
}

void DirectionLight::UpdateLight(float time)
{
	// 상수 버퍼에 데이터 전달...
	m_CBLight.direction = m_Direction;
	m_CBLight.diffuse = m_Diffuse;
	m_CBLight.ambient = m_Ambient;
	m_CBLight.Position = m_LitPos;
	m_CBLight.range = m_Range;

	// 
}
