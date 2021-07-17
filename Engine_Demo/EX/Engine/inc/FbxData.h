///
///
///
/// 1개의 노드의 데이터...
/// [2021/01/21 RUNSUMIN]
#pragma once
#include "FbxDataStructure.h"

class FbxData_Mesh
{
public:
	FbxData_Mesh();
	~FbxData_Mesh();
public:
	vector<Vertex> m_pVertexList;
	vector<Index> m_pIndexList;
	/// Ver_1.1 -> 노드별로 본 오프셋 생성
	vector<Matrix> m_BoneOffSet;


	Fbx_Material* m_pMaterial;	// 노드 하나의 재질 정보

	string m_NodeName;

	Matrix m_Worldm;
	Matrix m_GlobalBindPos;

	bool m_isBinding;
	bool m_TextureOn;
	bool m_NormalMapOn;
	bool m_MaskMapOn;
	bool m_ORMMapOn;

	int m_TextureCount;				// 로더에서 부여하는 텍스쳐 번호
	int m_NormalCount;				// 로더에서 부여하는 노멀 번호
	int m_MaskMapCount;				// 로더에서 부여하는 마스크맵 번호
	int m_ORMapCount;				// 로더에서 부여하는 ORM맵 번호
	int m_PolygonCount;			    // 이 노드의 폴리곤 개수
	int m_VertexCount;
	int m_IndexCount;


public:
	void CreateBiNorNTangent();
	void CalBinormalNTangent(int index1, int index2, int index3);
	void ReleaseFbxData();
};

