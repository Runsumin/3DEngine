#include "Fog.h"

Fog::Fog()
{

}

Fog::~Fog()
{

}

CBFOG Fog::GetFogData()
{
	return m_FogData;
}

void Fog::InitFog()
{
	m_FogData.Color = Vector4(1, 1, 1, 1);
	m_FogData.Density = 0.005f;
	m_FogData.OnOff = true;
}

void Fog::SetFogColor(Vector4 col)
{
	m_FogData.Color = col;
}

void Fog::SetFogDensity(float f)
{
	m_FogData.Density = f;
}

void Fog::SetFogOnOff(bool b)
{
	m_FogData.OnOff = b;
}
