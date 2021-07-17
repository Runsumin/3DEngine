///
///
///
///
/// 생성한 렌더타겟을 관리해주는 매니저
/// [2020/11/09 RUNSUMIN]
#pragma once

#include "DxDefine.h"
#include "D3DDevice.h"
#include "RenderTarget.h"
#include "UI.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"

#include "Fbx_MeshData.h"
#include "FbxDataStructure.h"

class RenderTargetMgr
{
public:
	RenderTargetMgr();
	~RenderTargetMgr();
public:
	ID3D11Device* m_pDevCopy;					// 디바이스 카피
	ID3D11DeviceContext* m_pDevConCopy;			// 디바이스 컨텍스트 카피

	RenderTarget* m_pRtResult;
	RenderTarget* m_pRt_Depth;					// 깊이 정보 전용 렌더타겟 - ZBuffer , depthBuffer
	RenderTarget* m_pRtBloomCurve;
	RenderTarget* m_pRtBloom_DownSmp_1;
	RenderTarget* m_pRtBloom_DownSmp_2;
	RenderTarget* m_pRtBloom_DownSmp_3;
	RenderTarget* m_pRtBloom_Result;
	RenderTarget* m_pRtBloom_Result_2;
	RenderTarget* m_pRtBloom_Result_3;
	RenderTarget* m_pRtBloom_Result_4;

	UI* m_pUI_Depth;
	UI* m_pUI_Result;
	UI* m_pUI_Bloom;
	UI* m_pUI_Bloom_DownSmp_1;
	UI* m_pUI_Bloom_DownSmp_2;
	UI* m_pUI_Bloom_DownSmp_3;
	UI* m_pUI_Bloom_Result;

	UI* m_pUI_Blur_step1;
	UI* m_pUI_Blur_step2;
	UI* m_pUI_Blur_step3;


	Matrix m_View;
	Matrix m_Proj;
	SIZE m_Size;
	SIZE m_ResultSize;
	SIZE m_DepthSize;
	SIZE m_BloomSize;
	SIZE m_BloomSize_smp1;
	SIZE m_BloomSize_smp2;
	SIZE m_BloomSize_smp3;

	bool m_RtDebugDraw;
	bool m_BloomOnOff;
	/// Shadow
	CBLIGHT m_Light;		// 라이트 정보 저장용...
	Matrix m_CamViewMat;	// 카메라 View 저장용
	Matrix m_CamProjMat;	// 카메라 Proj 저장용

public:
	void InitRenderTargetMgr();
	void SetViewPort(UINT w, UINT h);

	// Result
	void UpdateRT_Result();
	void ReStoreRT_Result();
	// Bloom
	void UpdateRT_Bloom();
	void ReStoreRT_Bloom();
	// Depth
	void UpdateRenderTarget_Depth();
	void RestoreRenderTarget_Depth();
	Matrix GetLightVPMatrix(Vector3 targetPos, float radious);

	// Debug용
	void DrawRt_Debug();

	// 렌더큐에서 최종으로 돌아갈 렌더타겟 Update
	void UpdateAllRenderTarget();
	// 화면 복구..
	void RestoreAllRenderTarget();

	// 렌더타겟 최종 텍스쳐 넘겨주는용... 임시
	ID3D11ShaderResourceView* GetShadowTex();
	ID3D11ShaderResourceView* GetDepthTex();
};

