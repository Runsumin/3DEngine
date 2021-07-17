#include "FbxData.h"

FbxData_Mesh::FbxData_Mesh()
	: m_PolygonCount(0), m_VertexCount(0), m_IndexCount(0), m_NodeName(""),
	m_Worldm(Matrix::Identity), m_GlobalBindPos(Matrix::Identity), m_isBinding(false)
{

}
FbxData_Mesh::~FbxData_Mesh()
{

}
void FbxData_Mesh::CreateBiNorNTangent()
{
	// 모델의 면 수 개산
	int facecount = m_VertexCount / 3;

	int index = 0;

	for (int i = 0; i < facecount; i++)
	{
		CalBinormalNTangent(index, index + 1, index + 2);
		index += 3;
	}
}

void FbxData_Mesh::CalBinormalNTangent(int index1, int index2, int index3)
{
	// 삼각형의 정보들
	Vector3 v1 = Vector3(m_pVertexList[index1].Pos.x,
		m_pVertexList[index1].Pos.y,
		m_pVertexList[index1].Pos.z);
	Vector3 v2 = Vector3(m_pVertexList[index2].Pos.x,
		m_pVertexList[index2].Pos.y,
		m_pVertexList[index2].Pos.z);
	Vector3 v3 = Vector3(m_pVertexList[index3].Pos.x,
		m_pVertexList[index3].Pos.y,
		m_pVertexList[index3].Pos.z);

	Vector2 uv1 = m_pVertexList[index1].Uv;
	Vector2 uv2 = m_pVertexList[index2].Uv;
	Vector2 uv3 = m_pVertexList[index3].Uv;

	// 현재 표면 두 벡터 계산
	Vector3 v2v1 = v2 - v1;
	Vector3 v3v1 = v3 - v1;
	// 텍스쳐 공간 벡터 계산
	Vector2 uv2uv1 = uv2 - uv1;
	Vector2 uv3uv1 = uv3 - uv1;
	// 분모 계산
	float den = 1.0f / (uv2uv1.x * uv3uv1.y - uv2uv1.y * uv3uv1.x);

	// TangnetVector, BiNormalVector
	Vector3 Tangent;
	Vector3 BiNormal;

	Tangent.x = (uv3uv1.y * v2v1.x - uv2uv1.y * v3v1.x) * den;
	Tangent.y = (uv3uv1.y * v2v1.y - uv2uv1.y * v3v1.y) * den;
	Tangent.z = (uv3uv1.y * v2v1.z - uv2uv1.y * v3v1.z) * den;

	BiNormal.x = (uv2uv1.x * v3v1.x - uv3uv1.x * v2v1.x) * den;
	BiNormal.y = (uv2uv1.x * v3v1.y - uv3uv1.x * v2v1.y) * den;
	BiNormal.z = (uv2uv1.x * v3v1.z - uv3uv1.x * v2v1.z) * den;

	float length = sqrt((Tangent.x * Tangent.x)
		+ (Tangent.y * Tangent.y)
		+ (Tangent.z * Tangent.z));

	Tangent.x = Tangent.x / length;
	Tangent.y = Tangent.y / length;
	Tangent.z = Tangent.z / length;

	length = sqrt((BiNormal.x * BiNormal.x)
		+ (BiNormal.y * BiNormal.y)
		+ (BiNormal.z * BiNormal.z));

	BiNormal.x = BiNormal.x / length;
	BiNormal.y = BiNormal.y / length;
	BiNormal.z = BiNormal.z / length;

	m_pVertexList[index1].Tangent = Tangent;
	m_pVertexList[index1].Binor = BiNormal;
	m_pVertexList[index2].Tangent = Tangent;
	m_pVertexList[index2].Binor = BiNormal;
	m_pVertexList[index3].Tangent = Tangent;
	m_pVertexList[index3].Binor = BiNormal;
}

void FbxData_Mesh::ReleaseFbxData()
{

}
