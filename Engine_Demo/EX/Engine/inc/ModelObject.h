//
//
//
// 파싱한 모델 데이터를 담을 클래스
// [2020/07/30 RUNSUMIN]
#pragma once
#include "DxDefine.h"
#include "AseModel.h"
#include "D3DDevice.h"
#include "Shader.h"
#include <vector>

class ModelObject
{
public:
	ModelObject();
	~ModelObject();

	AseModel* m_pObj;
	Shader* m_pShader;

	// 상수버퍼 데이터
	CBDEFAULT	m_CbDef;	// 행렬 정보
	CBMATERIAL  m_CbMtl;	// 재질 정보

	ID3D11Buffer* m_pCBDef;		//기본정보 운용용 상수버퍼.
	ID3D11Buffer* m_pCBMtl;		//재질정보 운용용 상수버퍼

	ID3D11Device* m_pDeviceCopy;
	ID3D11DeviceContext* m_pDeviceCtCopy;

	// 실제 모델 조작용
	XMMATRIX m_UserTm;
	XMMATRIX m_Transm;
	XMMATRIX m_Rotm;
	XMMATRIX m_Scalem;

	Vector3 m_vPos;
	Vector3 m_vRot;
	Vector3 m_vScale;




public:
	int LoadModelFromFile(TCHAR* filename, ID3D11Device* pdevice , ID3D11DeviceContext* pDeviceContext);
	int initModel();								// 모델에 필요한 디바이스 장치를 가져옵니다.
	int initShader();								// 모델에 필요한 셰이더 정보를 가져옵니다.
	bool ModelUpdate(float dTime);
	bool ModelDraw(float dTime);

	void ModelRelease();
};

