///
///
///
/// FBX 모델 클래스
/// [2020/10/14 RUNSUMIN]
#pragma once
#include "FbxDataStructure.h"
#include "FbxData.h"
#include "D3DDevice.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "EngineInterface.h"
#include "Geometry_Generater.h"
#include "DxDefine.h"
#include "MeshFilter.h"
//template <typename T>
//void deletevector(vector<T*> temp)
//{
//	int size = temp.size();
//
//	for (int i = 0; i < size; i++)
//	{
//		delete temp[i];
//	}
//}

// 모델 클래스
class FBX_StaticMesh : public MeshFilter
{
public:
	FBX_StaticMesh();
	~FBX_StaticMesh();
private:
	// 모델 데이터 클래스
	class FbxModelData
	{
	public:
		FbxData_Mesh* m_pFbxData;
		ID3D11Buffer* m_pVB;
		ID3D11Buffer* m_pIB;
	};

public:
	/////////////////////////////////	데이터...
	vector<FbxData_Mesh*> m_pFbxDataList;	// 이부분을 자체 포멧으로 변환 필요... // mesh 
	vector<Bone*> m_pBoneList_Hierarchy;	// 이부분을 자체 포멧으로 변환 필요... // bone
	vector<Bone*> m_pAllBoneList;			// 이부분을 자체 포멧으로 변환 필요... // bone
	vector<AnimationData*> m_pAniDataList;	// 이부분을 자체 포멧으로 변환 필요... // animation

	//vector<Fbx_Texture*> m_pFbxTexure;
	vector<string> m_Texfilename;
	vector<string> m_Norfilename;
	vector<string> m_MaskMapfilename;
	vector<ID3D11ShaderResourceView*> m_FbxModelTexture;
	vector<ID3D11ShaderResourceView*> m_FbxModelNormal;
	vector<ID3D11ShaderResourceView*> m_FbxModelMaskMap;
	string m_TexPath;
	int m_TexCount;
	int m_NorCount;
	int m_MaskCount;
	int m_BoneCount;

	virtual void InitFbxModel() override;
	virtual void FbxModelUpdate(float time) override;
	virtual void FbxModelDraw(float time) override;
	virtual void FbxModelInfo() override;
	virtual void ReleaseFbxModel() override;
	virtual void SetUserMatrix(Matrix usertm) override;
	virtual string GetModelName() override;
	
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

	// 본 정보 확인용 박스
	//vector<Geometry_Generater*> m_pBonedebugList;

private:
	// 모델 Init
	void CreateHWBuffer();
	void InitShader();
	void SetTextureDateInit();
	// 모델 Update
	void ModelAniUpdate(float time, int anicount, int keycount);
	void ModelAnimationUpdate_Recur_Temp(int anicount, Bone* pbone, float currtime, Matrix parenttm, vector<Matrix>& bonematdata, int nowframecount);
	int CalAniFrame(float Time);
	// 모델 Draw
	void SetPrimitiveData(int nodecount);
	void SetTextrueData(int nodecount);
	void SetMaterialData(int nodecount);
	void SetConstantBuffer(int nodecount);
	void Draw(int nodecount);

public:
	// 모델 이름
	string m_ModelName;
	// 실제 오브젝트 조작용
	Matrix m_UserTm;

	// 축 교체용...
	Matrix m_AxisChange;

	// 애니메이션 진행 속도
	float m_AnimationSpeed;

	// 예외처리
	bool m_bNorOn;
	bool m_TexOn;
	bool m_AniOn;
	bool m_SKinOn;
	bool bLightWind;

private:
	vector<FbxModelData*> m_pFinalDatalist;	// 자체 포맷 재가공...

	ID3D11Device* m_pDevCopy;				// 디바이스 카피
	ID3D11DeviceContext* m_pDevConCopy;		// 디바이스 컨텍스트 카피
	Shader* m_pShader;

	CBFBXDEF    m_CbDef;					// 기본 운용 행렬 정보
	CBFBXMAT	m_CbMat;					// FBX 전용 재질 정보
	CBLIGHT		m_CbLight;					// FBX 전용 재질 정보

	ID3D11Buffer* m_pCbDefBuf;
	ID3D11Buffer* m_pCbMatBuf;
	ID3D11Buffer* m_pCbLightBuf;

	Matrix m_CamViewMat;
	Matrix m_CamProjMat;
	Matrix m_LightVP;

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

	bool m_AniDown;

};
