///
///
///
/// 광원 정보 클래스
/// 단순히 데이터를 셋팅해주기만 하고 외부로 데이터 넘겨준다...
/// [2020/11/03 RUNSUMIN]
#pragma once

#include "D3DDevice.h"	// 구조체 때문에... 따로 분리 해야 함
#include "EngineInterface.h"

class DirectionLight : public GRAPHICENGINE::IDirectionLightInterface
{
public:
	DirectionLight();
	~DirectionLight();
private:
	GRAPHICENGINE::IDirectionLightInterface::IDirectrionLitData m_Data;
public:
	GRAPHICENGINE::IDirectionLightInterface::IDirectrionLitData GetDirectionLitData();
private:
	virtual void InitializeDirectrionLight(Vector4 dir, Vector4 dif, Vector4 amb) override;
	virtual void SetDirectrionLightDirection(Vector4 dir) override;
	virtual void SetDirectrionLightDiffuse(Vector4 dif) override;
	virtual void SetDirectrionLightAmbient(Vector4 amb) override;
};

class PointLight : public GRAPHICENGINE::IPointLightInterface
{
public:
	PointLight();
	~PointLight();
private:
	GRAPHICENGINE::IPointLightInterface::IPointLitData m_Data;
public:
	GRAPHICENGINE::IPointLightInterface::IPointLitData GetPointLitData();
private:
	virtual void InitializePointLight(Vector4 dif, Vector4 amb, Vector3 pos, float range) override;
	virtual void SetPointLightDIffuse(Vector4 dif) override;
	virtual void SetPointLightAmbient(Vector4 amb) override;
	virtual void SetPointLightPosition(Vector3 pos) override;
	virtual void SetPointLightRange(float range) override;
};

