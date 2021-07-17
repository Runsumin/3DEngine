///
///
///
///
///
/// 안개 효과 클래스
/// [2021/06/25/RUNSUMIN]
#pragma once
#include "D3DDevice.h"

class Fog 
{
public:
	Fog();
	~Fog();
public:
	CBFOG m_FogData;
public:
	CBFOG GetFogData();
public:
	void InitFog();
	void SetFogColor(Vector4 col);
	void SetFogDensity(float f);
	void SetFogOnOff(bool b);


};

