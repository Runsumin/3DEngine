///
///
///
/// 랜더링 순서&방식 관리 클래스
/// 하나의 모델 안에서도 렌더링 방식을 바꿔줘야 되는 경우가 생긴다...
/// [2021/04/06 RUNSUMIN]

#pragma once
#include "EngineInterface.h"
#include "DxDefine.h"
#include "D3DDevice.h"
#include "RenderTargetMgr.h"
#include "Light.h"
#include "Fog.h"

class Render_Queue
{
public:
	Render_Queue();
	~Render_Queue();
private:
	/// 렌더타겟 매니저..
	RenderTargetMgr* m_pRtMgr;
	Fog* m_pFog;

public:
	// Skybox
	GRAPHICENGINE::ISkyBoxInterface* m_pSkybox;
	// BackGround
	std::vector<GRAPHICENGINE::ITerrain_Interface*> m_pTerrain;
	// Mesh Object
	std::vector<GRAPHICENGINE::IModelInterface*> m_pDrawModelList;
	// UI Object
	std::vector<GRAPHICENGINE::IUiInterface*> m_pDrawUIList;

	/// Initialize
	void GetSkyBoxPointer(GRAPHICENGINE::ISkyBoxInterface* obj);
	void PushBackTerrainDrawList(GRAPHICENGINE::ITerrain_Interface* obj);
	void PushBackModelDrawList(GRAPHICENGINE::IModelInterface* obj);
	void PushBackUIDrawList(GRAPHICENGINE::IUiInterface* obj);

	/// Draw
	void DrawAll(float time);
	void AllModelDraw(float time);
	void ShadowDraw(float time);

	// Sorting
	void GetDataOneTime();
	void SkyBoxDraw();
	void BackGroundDataDraw();

	/// RenderTarget
	void RenderTargetPass(float time);

	/// Queue Reset
	void ResetRenderQueue_Model();
	void ResetRenderQueue_UI();
	void DeleteQueueList(int index);

	/// Culling
	void SetCulling(bool b);


	/// Light - 모든 라이트는 여러개 생성 가능하다.
	std::vector<DirectionLight*> m_DirectionLightList;
	std::vector<PointLight*>	 m_PointLightList;
public:
	/// Shadow
	Vector3 m_TargetPosition;
	float m_ShadowRadious;
	bool m_ShadowOn;
	Matrix m_LightViewProjMat;

	/// Culling
	//1 . View Frustum Culling
	bool ViewFrustumCulling(Vector3 objworldpos, Matrix viewproj, float radious);
	float CmpPlaneAndDot(Vector4 plane, Vector3 dot);
	Matrix m_CamViewProjMat;
	bool m_bCulling;
	int m_DrawColl;

	///FOG
	void SetFogOnOff(bool b);
	void SetFogColor(Vector4 col);
	void SetFogDensity(float f);

	///Bloom
	bool m_bBloom;

	/// Channel View
	CHANNELVIEW m_ChannelView;
};

