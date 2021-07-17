///
///
///
/// SkinnedMesh
/// [2021/04/13 RUNSUMIN]
#pragma once
#include "Fbx_MeshData.h"
#include "EngineInterface.h"
#include "MeshFilter.h"
#include "ShaderMgr.h"
#include "Geometry_Generater.h"
#include <map>
class SKinned_Mesh : public MeshFilter
{
public:
	SKinned_Mesh();
	~SKinned_Mesh();

private:
	// 모델 정보 포인터...
	Fbx_MeshData* m_pModelData;
private:
	// 모델 데이터 클래스
	class FbxModelData
	{
	public:
		FbxData_Mesh* m_pFbxData;
		ID3D11Buffer* m_pVB;
		ID3D11Buffer* m_pIB;
		GRAPHICENGINE::RenderQueueSort m_QueueSrot;
	};

private:
	// 외부에 Bone Matrix 넘기기 위한 저장용
	vector<Matrix> m_AniBoneMatData;

public:
	// 실제 오브젝트 조작용
	Matrix m_UserTm;

	// 애니메이션 진행 속도
	float m_AnimationSpeed;

	// 예외처리
	bool m_bNorOn;
	bool m_AniOn;
	bool bLightWind;

private:
	vector<FbxModelData*> m_pFinalDatalist;	// 자체 포맷 재가공...

	ID3D11Device* m_pDevCopy;				// 디바이스 카피
	ID3D11DeviceContext* m_pDevConCopy;		// 디바이스 컨텍스트 카피

	CBFBXDEF    m_CbDef;					// 기본 운용 행렬 정보
	CBFBXMAT	m_CbMat;					// FBX 전용 재질 정보
	CBLIGHT		m_CbLight;					// FBX 전용 조명 정보
	CBFOG		m_CBFog;					// 안개 정보
	CHANNELVIEW m_ChannelView;				// 디버그용 채널 뷰

	ID3D11Buffer* m_pCbDefBuf;
	ID3D11Buffer* m_pCbMatBuf;
	ID3D11Buffer* m_pCbLightBuf;
	ID3D11Buffer* m_pCbFogBuf;
	ID3D11Buffer* m_pCbChannelViewBuf;

	Matrix m_CamViewMat;
	Matrix m_CamProjMat;
	Matrix m_LightVP;
	/// Shader

private:
	Shader* m_pShader;
	/// 뎁스 픽셀 셰이더
	ID3D11PixelShader* m_pPsDepth;

private:
	Matrix m_Trans;
	Matrix m_Rot;
	Matrix m_Scale;

	Vector3 m_vPos;
	Vector3 m_vRot;
	Vector3 m_vScl;

	// 애니메이션
	float m_AniTime;			// 누적값
	float m_CurrTime;			// 현재값;
	bool m_AniOnOff;
	int m_NowAniCount;
	bool m_Aniloop;
	int m_NowFrameCount;

	int m_NowNodeCount;

	bool m_AniDown;

	/// Material 연동
	bool m_TexOn;
	bool m_NormalOn;
	bool m_MaskOn;
	bool m_BumpMappingOn;
	bool m_PBROn;

	vector<ID3D11ShaderResourceView*> m_AlbedoMapList;
	vector<ID3D11ShaderResourceView*> m_NormalMapList;
	vector<ID3D11ShaderResourceView*> m_MaskMapList;
	vector<ID3D11ShaderResourceView*> m_ORMMapList;
	vector<ID3D11ShaderResourceView*> m_EmissiveMapList;

	// RenderState
	GRAPHICENGINE::RenderState m_ModelRenderState;

	// Collision
	float m_Radious;

	GRAPHICENGINE::IGeoMetry_InterFace* m_CollBox;
	vector<GRAPHICENGINE::IGeoMetry_InterFace*> m_Bone;

private:
	// 모델 Init
	void CreateHWBuffer();
	// material 에서 Textrue 데이터 받아오기
	void GetShaderResource();

	// 모델 Update
	void ModelAniUpdate(float time, int anicount, int keycount);
	void ModelAnimationUpdate_Recur_Temp(int anicount, Bone* pbone, float currtime, Matrix parenttm, vector<Matrix>& bonematdata, int nowframecount);
	int CalAniFrame(float Time);
	// 모델 Draw
	void SetPrimitiveData(int nodecount);
	void SetTextrueData(int nodecount);
	void SetMaterialData(int nodecount);
	void SetConstantBuffer(int nodecount);
	void SetConstantBuffer_Depth(int nodecount, Matrix shadow);
	void Draw(int nodecount);

public:
	bool SetModelData(Fbx_MeshData* pdata);

	virtual void InitFbxModel() override;
	virtual void InitModelTexture() override;
	virtual void FbxModelUpdate(float time) override;
	virtual void FbxModelDraw(float time, int nodecount, RenderPath path, Matrix shadow) override;
	virtual void FbxModelInfo() override;
	virtual void ReleaseFbxModel() override;
	virtual void SetUserMatrix(Matrix usertm) override;
	virtual Matrix GetUserMatrix() override;
	virtual string GetModelName() override;
	virtual string GetNodeName(int nodecount) override;
	virtual int GetModelNodeCount() override;
	virtual GRAPHICENGINE::RenderQueueSort GetModelNodeRendersort(int nodecount) override;
	virtual void SetModelNodeRenderQueuesort(int nodecount, GRAPHICENGINE::RenderQueueSort sort) override;
	virtual void SetDrawScreen(bool draw) override;
	virtual bool GetDrawScreen() override;
	virtual void SetModelRenderState(GRAPHICENGINE::RenderState state) override;
	virtual Vector3 GetWorldModelPosition() override;
	virtual float GetCollSphereRadious() override;
	virtual GRAPHICENGINE::IModelInterface::ModelSort GetModelSort() override;

	/// Animation
	virtual void SetAnimationSpeed(float speed) override;
	virtual void ChangeAnimation(int count) override;
	virtual void SetAnimationLoop(bool loop) override;
	virtual float GetAniTotalTime() override;
	virtual bool GetAniEndTimeing() override;
	virtual int GetAniTotalKeyCount() override;
	virtual int GetAniNowKeyCount()	override;
	virtual void SetNowKeyCount(int key) override;
	virtual void SetAniOn() override;
	virtual void SetAniOff() override;
	virtual Matrix GetHiearchyMatrixByName(string name) override;
	virtual vector<string> GetBoneOffSetName() override;
	
	/// Material
	virtual void SetMaterialData(GRAPHICENGINE::IMaterialInterface* pobj, string nodename, GRAPHICENGINE::IMaterialInterface::TextureSort sort) override;
	virtual void SetEmissiveMapOnOff(bool b) override;
	virtual void AddEmissiveMap(string filename) override;
	virtual void ChangeApplyEmissiveMap(int index) override;

	/// Light Info
	virtual void SetDirectionLightData(GRAPHICENGINE::IDirectionLightInterface::IDirectrionLitData data) override;
	virtual void SetPointLightData(GRAPHICENGINE::IPointLightInterface::IPointLitData data) override;


};

