#include "Light.h"

/// DirectionLight
DirectionLight::DirectionLight()
{

}

DirectionLight::~DirectionLight()
{

}

GRAPHICENGINE::IDirectionLightInterface::IDirectrionLitData DirectionLight::GetDirectionLitData()
{
	return m_Data;
}

void DirectionLight::InitializeDirectrionLight(Vector4 dir, Vector4 dif, Vector4 amb)
{
	dir.Normalize();
	m_Data.Direction = dir;
	m_Data.Diffuse = dif;
	m_Data.Ambient = amb;
}

void DirectionLight::SetDirectrionLightDirection(Vector4 dir)
{
	dir.Normalize();
	m_Data.Direction = dir;
}

void DirectionLight::SetDirectrionLightDiffuse(Vector4 dif)
{
	m_Data.Diffuse = dif;
}

void DirectionLight::SetDirectrionLightAmbient(Vector4 amb)
{
	m_Data.Ambient = amb;
}

/// Point Light
PointLight::PointLight()
{

}

PointLight::~PointLight()
{

}

GRAPHICENGINE::IPointLightInterface::IPointLitData PointLight::GetPointLitData()
{
	return m_Data;
}

void PointLight::InitializePointLight(Vector4 dif, Vector4 amb, Vector3 pos, float range)
{
	m_Data.Diffuse = dif;
	m_Data.Ambient = amb;
	m_Data.Position = pos;
	m_Data.Range = range;
}

void PointLight::SetPointLightDIffuse(Vector4 dif)
{
	m_Data.Diffuse = dif;
}

void PointLight::SetPointLightAmbient(Vector4 amb)
{
	m_Data.Ambient = amb;
}

void PointLight::SetPointLightPosition(Vector3 pos)
{
	m_Data.Position = pos;
}

void PointLight::SetPointLightRange(float range)
{
	m_Data.Range = range;
}
