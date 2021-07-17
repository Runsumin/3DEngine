//
//
//
// 디버깅용 축 만들어주기
// [2020/08/07 RUNSUMIN]
#pragma once
// 화면에 그려주기 위한 필수 헤더
#include "DxDefine.h"
#include "shader.h"
#include "Camera.h"
#include "EngineInterface.h"

class Axis : public GRAPHICENGINE::IGizmo_InterFace
{
public:
	struct VTX_AXIS
	{
		float x, y, z;
		float r, g, b, a;
	};

	_declspec(align(16)) struct CBAXIS
	{
		XMMATRIX mWorld;
		XMMATRIX mView;
		XMMATRIX mProj;
		XMMATRIX mWVP;
	};

public:
	Axis();
	~Axis();

public:
	ID3D11Device* m_pDeviceCopy;
	ID3D11DeviceContext* m_pDeviceContextCopy;
	
	CBAXIS m_cbAxis;

	Shader* m_pShader;

	ID3D11Buffer* m_pAxisbuf;
	ID3D11Buffer* m_pVb;

	virtual int InitAxis() override;
	virtual int Update(Matrix world) override;
	virtual int Render() override;
	virtual void Release() override;
};

