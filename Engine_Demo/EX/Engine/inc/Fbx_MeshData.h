///
///
///
/// FBX_MeshData class
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
#include "Material.h"

template <typename T>
void deletevector(vector<T*> temp)
{
	int size = temp.size();

	for (int i = 0; i < size; i++)
	{
		delete temp[i];
	}
}

// 모델 클래스
class Fbx_MeshData
{
public:
	Fbx_MeshData();
	~Fbx_MeshData();
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

public:
	/////////////////////////////////	데이터...
	vector<FbxData_Mesh*> m_pFbxDataList;	// 이부분을 자체 포멧으로 변환 필요... // mesh 
	vector<Bone*> m_pBoneList_Hierarchy;	// 이부분을 자체 포멧으로 변환 필요... // bone_Hierarchy
	vector<Bone*> m_pAllBoneList;			// 이부분을 자체 포멧으로 변환 필요... // bone_All
	vector<AnimationData*> m_pAniDataList;	// 이부분을 자체 포멧으로 변환 필요... // animation

	// Texture Name
	vector<string> m_Texfilename;
	vector<string> m_Norfilename;
	vector<string> m_MaskMapfilename;
	vector<string> m_ORMfilename;
	vector<string> m_LinkNodeNameList;

	// Texture Path
	string m_TexPath;

	// 텍스쳐 재질 정보 리스트 - 텍스쳐 개수만큼 생성...
	vector<Material*> m_pMaterialList;
	bool m_AutoTextureSet; // - 데이터 안에 텍스쳐 경로가 정상적으로 세팅되어 있을 때만 true

	/// Loader 에서만 사용되는 데이터
	int m_BoneCount;
	int m_TexCount;
	int m_NorCount;
	int m_MaskCount;
	int m_ORMCount;

	/// 충돌 처리용...
	float m_BiggstX;
	float m_BiggstY;
	float m_BiggstZ;

	float m_leastX;
	float m_leastY;
	float m_leastZ;

public:
	// 모델 이름
	string m_ModelName;
	// 축 교체용...
	Matrix m_AxisChange;
	// 스킨 존재 여부 확인
	bool m_SKinOn;
	//
	MeshFilter::eMeshfiltersort m_MeshFilterSort;

public:
	void SetMaterialDataAuto();
	void ReleaseModelData();

};
