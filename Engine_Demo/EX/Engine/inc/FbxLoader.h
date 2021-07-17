///
///
///
/// FBX SDK를 이용한 로더
/// [2021/02/08] 싱글턴으로 변경
/// [2021/04/01] 싱글턴에서 다시 오브젝트로 변경
/// [2020/10/12 RUNSUMIN]
#pragma once
#include "FbxData.h"
#include "Fbx_MeshData.h"
#include "EngineInterface.h"
#include <map>


class Fbx_MeshData;

class FbxLoader
{
	friend class FbxData_Mesh;

public:
	FbxLoader();
	~FbxLoader();

	// 한번 로드한 데이터는 들고있자.. key, value
	std::map<std::string, Fbx_MeshData*> m_ImportDatalist;


	///  하나의 모델을 불러 올 때.... 여러개의 모댈을 불러 올 때는 생각중
	FbxManager* m_fbxMgr;			// Fbx 매니저
	FbxScene* m_Scene;				// FBX 씬
	FbxDocumentInfo* m_SceneInfo;	// 씬 정보
	FbxIOSettings* m_Ios;			// I/O 설정 객채(씬정보에서 파일 가져오거나 보낼지 여부 지정). 싱글턴으로 변경
	FbxImporter* m_FbxImporter;
	FbxAxisSystem m_AxisSystem;

	///
	Matrix m_AxisChange;

public:
	/// 로더 셋팅
	bool InitializeFbxLoader();
	void SceneSetting(FbxScene* Scene);
	void ReleaseFbxLoader();


	/// 데이터 로딩(노드)
	Fbx_MeshData* LoadModel(string Filename);
	Fbx_MeshData* SetModel(string Filename);
	void SetTextrueDate();

	/// 애니메이션만 로드
	AnimationData* LoadAnimationDataStack(string Filename);
	void GetHierarchydata(FbxNode* rootnode, vector<Bone*>& bonelist, Bone* parentbone);

	/// 데이터 로딩(노드)
	void LoadNode(FbxNode* pNode, Fbx_MeshData* pmodel, FbxPose* pose);		// 모델 기하정보 로드
	void LoadSkeletone(FbxNode* pNode, Fbx_MeshData* pmodel, Bone* parentbone, FbxPose* pose);		// 모델 기하정보 로드
	void GetTexturePath(string filename, FbxScene* scene, Fbx_MeshData* pmodel);
	void CalAxisMaxToDirectX(FbxAxisSystem maxAxisSystem);

	/// 애니메이션
	AnimationData* LoadAnimationData(FbxScene* pscene, vector<Bone*> HirearchyData);
	void GetAnimationData_Recur(FbxNode* prootnode, AnimationData* panidata, vector<Bone*> HirearchyData);
	void GetAnimationMatrix(FbxNode* pNode, AnimationData* panidata, vector<Bone*> HirearchyData);

	/// 버텍스 정보
	void GetVertex_List(FbxNode* pNode, FbxData_Mesh* pdata, Fbx_MeshData* pmodel);
	Vector2 GetUV(FbxMesh* pmesh, int i, int j);
	Vector3 GetNormal(FbxMesh* pmesh, int i, int j);
	Vector3 GetNormaldata(FbxMesh* pmesh, int indexcount, int vertexcount);
	Vector3 GetBiNormal(FbxMesh* pmesh, int indexcount, int vertexcount);
	Vector3 GetTangent(FbxMesh* pmesh, int indexcount, int vertexcount);
	void LoadBoneIndexWeight(FbxNode* pnode, vector<BoneIndexWeight>& dummybonelist, FbxData_Mesh* pdata, Fbx_MeshData* pmodel);
	void GetWorldMatrix(FbxNode* pnode, FbxData_Mesh* pdata, FbxPose* pose);

	/// 재질 정보
	void GetMaterials(FbxNode* pnode, FbxData_Mesh* pdata, Fbx_MeshData* pmodel);
	void GetMaterialAttribute(FbxSurfaceMaterial* pmat, Fbx_Material* matdata);
	void GetMaterialTexture(FbxSurfaceMaterial* pmat, FbxData_Mesh* pdata, Fbx_MeshData* pmodel);

	/// 본 정보
	void GetSkeletonData(FbxNode* pnode, Bone* pBonedata, FbxNodeAttribute::EType type, int BoneIndex);
	int FindBoneIndex(string name, vector<Bone*> HirearchyData);

	/// 디버그용
	void GetFileName(const TCHAR* FullPath, TCHAR* FileName);
	void GetPath(const TCHAR* FullPathName, TCHAR* Path);
	Matrix ConvertFbxMattoDirxMat(FbxMatrix fbxmat);
	TCHAR* ConvertStringToTCHAR(string& s);
	string ConvertTCharToString(const TCHAR* ptsz);
	FbxAMatrix GetGeometryTransformation(FbxNode* pnode);
	Vector2 ConvertVec2(FbxVector2 vec2);
	Vector3 ConvertVec3(FbxVector4 vec3);
	Vector4 ConvertVec4(FbxVector4 vec4);

	void SortWeightData(vector<BoneIndexWeight>& bonelist);

public:
	string GetNodeName(FbxNode* pNode);
	int GetPolygonCnt(FbxNode* pNode);
	void SetModelInfo();
};
