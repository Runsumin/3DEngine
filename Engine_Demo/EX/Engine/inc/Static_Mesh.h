///
///
///
/// Static Mesh
/// [2021/04/13 RUNSUMIN]
#pragma once
#include "Fbx_MeshData.h"
#include "EngineInterface.h"
#include "MeshFilter.h"
#include "ShaderMgr.h"
#include <Effects.h>

class Static_Mesh : public MeshFilter
{
public:
	Static_Mesh();
	~Static_Mesh();
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
		ID3D11Buffer* m_InsB;
		GRAPHICENGINE::RenderQueueSort m_QueueSrot;
	};
	// Instance Draw
	struct Instancetype
	{
		Vector3 Pos;
		Vector3 Rot;
		Vector3 Scl;
	};
	vector<Instancetype> m_InstanceList;
public:
	// 실제 오브젝트 조작용
	Matrix m_UserTm;

private:
	vector<FbxModelData*> m_pFinalDatalist;	// 자체 포맷 재가공...

	ID3D11Device* m_pDevCopy;				// 디바이스 카피
	ID3D11DeviceContext* m_pDevConCopy;		// 디바이스 컨텍스트 카피

	/// Shader
	Shader* m_pShader;
	ID3D11PixelShader* m_pPsDepth;	/// 뎁스 픽셀 셰이더

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

	vector<ID3D11ShaderResourceView*> m_AlbedoMapList;
	vector<ID3D11ShaderResourceView*> m_NormalMapList;
	vector<ID3D11ShaderResourceView*> m_MaskMapList;
	vector<ID3D11ShaderResourceView*> m_ORMMapList;
	vector<ID3D11ShaderResourceView*> m_EmissiveMapList;

	// RenderState
	GRAPHICENGINE::RenderState m_ModelRenderState;

	// Collision
	float m_Radious;
private:
	Matrix m_Trans;
	Matrix m_Rot;
	Matrix m_Scale;

	Vector3 m_vPos;
	Vector3 m_vRot;
	Vector3 m_vScl;
	int m_NowNodeCount;

	bool m_EmssiveprevState;

	int m_EmissiveMapCount;

	///PBR
private:
	std::unique_ptr<PBREffect> effect;

	GRAPHICENGINE::IGeoMetry_InterFace* m_CollBox;

private:
	// 모델 Init
	void CreateHWBuffer();
	// material 에서 Textrue 데이터 받아오기
	void GetShaderResource();

	// 모델 Update

	// 모델 Draw
	void SetPrimitiveData(int nodecount);
	void SetTextrueData(int nodecount);
	void SetMaterialData(int nodecount);
	void SetConstantBuffer(int nodecount);
	void SetConstantBuffer_Depth(int nodecount, Matrix shadow);
	void Draw(int nodecount);

public:
	// 인스턴싱 데이터 넣기.
	void AddInstanceData(Vector3 pos, Vector3 rot, Vector3 scl);
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
	virtual GRAPHICENGINE::IModelInterface::ModelSort GetModelSort() override;

	/// 애니메이션 전용... 사용X 
	virtual void SetAnimationSpeed(float speed) override;		// 스피드 설정
	virtual void ChangeAnimation(int count) override;			// 애니메이션 전환
	virtual float GetAniTotalTime() override;					// 애니메이션 총 시간
	virtual bool GetAniEndTimeing() override;					// 애니메이션 마지막 타임 받아옴
	virtual int GetAniTotalKeyCount() override;					// 애니메이션 총 프레임
	virtual int GetAniNowKeyCount()	override;					// 애니메이션 현채 
	virtual void SetNowKeyCount(int key) override;				// 현재 키 설정
	virtual void SetAniOn() override;							// 애니메이션 ON;
	virtual void SetAniOff() override;							// 애니메이션 OFF
	virtual void SetAnimationLoop(bool loop) override;
	virtual Matrix GetHiearchyMatrixByName(string name) override;
	virtual vector<string> GetBoneOffSetName() override;		// BoneOffSet
	virtual float GetCollSphereRadious() override;

	// Material
	virtual void SetMaterialData(GRAPHICENGINE::IMaterialInterface* pobj, string nodename, GRAPHICENGINE::IMaterialInterface::TextureSort sort) override;
	virtual void SetEmissiveMapOnOff(bool b) override;
	virtual void AddEmissiveMap(string filename) override;
	virtual void ChangeApplyEmissiveMap(int index) override;

	//Light Info
	virtual void SetDirectionLightData(GRAPHICENGINE::IDirectionLightInterface::IDirectrionLitData data) override;
	virtual void SetPointLightData(GRAPHICENGINE::IPointLightInterface::IPointLitData data) override;


};

