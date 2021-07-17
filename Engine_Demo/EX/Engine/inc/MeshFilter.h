///
///
///
/// Mesh Renderer 이전 메쉬 필터
/// SKinnedMesh, Static Mesh 구분.
/// [2021/04/07 RUNSUMIN]
#pragma once
#include "EngineInterface.h"
using namespace std;
using namespace DirectX::SimpleMath;

class MeshFilter : public GRAPHICENGINE::IModelInterface
{
public:
	enum class eMeshfiltersort
	{
		SkinnedMesh, StaticMesh
	};
public:
	MeshFilter();
	~MeshFilter();
public:
	// 원래는 임포터에서 메쉬구분이 되야하지만... 작업이 너무 커져서 임의 지정으로 변경
	eMeshfiltersort m_MeshFilterSort;
	// 화면에 그릴지 말지...
	bool m_Draw;

protected:
	virtual void InitFbxModel() abstract;
	virtual void InitModelTexture() abstract;
	virtual void FbxModelUpdate(float time) abstract;
	virtual void FbxModelDraw(float time, int nodecount, RenderPath path, Matrix shadow) abstract;
	virtual void FbxModelInfo() abstract;
	virtual void ReleaseFbxModel() abstract;
	virtual void SetUserMatrix(Matrix usertm) abstract;
	virtual Matrix GetUserMatrix() abstract;
	virtual string GetModelName() abstract;
	virtual string GetNodeName(int nodecount) abstract;
	virtual int GetModelNodeCount() abstract;
	virtual GRAPHICENGINE::RenderQueueSort GetModelNodeRendersort(int nodecount) abstract;
	virtual void SetModelNodeRenderQueuesort(int nodecount, GRAPHICENGINE::RenderQueueSort sort) abstract;		// 모델 노드당 렌더 순서 지정
	virtual void SetDrawScreen(bool draw) abstract;
	virtual bool GetDrawScreen() abstract;
	virtual void SetModelRenderState(GRAPHICENGINE::RenderState state) abstract;
	virtual Vector3 GetWorldModelPosition() abstract;
	virtual float GetCollSphereRadious() abstract;
	virtual GRAPHICENGINE::IModelInterface::ModelSort GetModelSort() abstract;


	// 애니메이션 전용... 분리 필요
	virtual void SetAnimationSpeed(float speed) abstract;			// 스피드 설정
	virtual void ChangeAnimation(int count) abstract;				// 애니메이션 전환
	virtual float GetAniTotalTime() abstract;						// 애니메이션 총 시간
	virtual bool GetAniEndTimeing() abstract;						// 애니메이션 마지막 타임 받아옴
	virtual int GetAniTotalKeyCount() abstract;						// 애니메이션 총 프레임
	virtual int GetAniNowKeyCount()	abstract;						// 애니메이션 현채 
	virtual void SetNowKeyCount(int key) abstract;					// 현재 키 설정
	virtual void SetAniOn() abstract;								// 애니메이션 ON;
	virtual void SetAniOff() abstract;								// 애니메이션 OFF
	virtual void SetAnimationLoop(bool loop) abstract;				// Loop 설정
	virtual Matrix GetHiearchyMatrixByName(string name) abstract;	// 본 정보 Get
	virtual vector<string> GetBoneOffSetName() abstract;		// BoneOffSet

	// Material
	virtual void SetMaterialData(GRAPHICENGINE::IMaterialInterface* pobj, string nodename, GRAPHICENGINE::IMaterialInterface::TextureSort sort) abstract;
	virtual void SetEmissiveMapOnOff(bool b) abstract;
	virtual void AddEmissiveMap(string filename) abstract;
	virtual void ChangeApplyEmissiveMap(int index) abstract;

	//Light Info
	virtual void SetDirectionLightData(GRAPHICENGINE::IDirectionLightInterface::IDirectrionLitData data) abstract;
	virtual void SetPointLightData(GRAPHICENGINE::IPointLightInterface::IPointLitData data) abstract;



/// Mesh Information
public:

};

