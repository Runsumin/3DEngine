#pragma once

#include "DxDefine.h"
#include "vector"
#include "AseNode.h"
#include "AseParser.h"
#include "locale.h"	
#include "Collision2D.h"
struct MODEL_MATERIAL
{
	TCHAR	m_MtlTexName[256];	// 재질 정보 안 텍스쳐 파일명 
	TCHAR	m_Texname[256];	// 재질 정보 안 텍스쳐 파일명 
	XMFLOAT4 m_Diffuse;			// 확산광
	XMFLOAT4 m_Ambient;			// 주변광
	XMFLOAT4 m_Specular;		// 스펙큘러
	FLOAT	 m_SpcePower;		// 스펙큘러 파워

	UINT	m_MtlCount;			// 재질 개수

	ID3D11ShaderResourceView* m_pTexture;	// 실제 텍스쳐

	BOOL	m_bTexOn;

};
class Collision2D;

class AseModel
{
public:
	// 생성자 & 소멸자 
	AseModel();
	~AseModel();
public:

	TCHAR	m_FileName[256];	//파일명 
	TCHAR	m_NodeName[256];	//노드(메쉬) 이름 

	MODEL_MATERIAL* m_ModelMtlList;		// 재질 정보 리스트

	std::vector<AseNode*> m_NodeList;	// 노드 정보 리스트
	std::vector<AseNode*> m_BoneList;	// 뼈대 정보 리스트
	std::vector<AseNode*> m_CollList;	// 충돌 정보 리스트
	AseNode* m_Weapon;


	AseNode* m_RootNode;				// 루트

	std::vector<AseNode*> m_RootNodeList;//

	int m_MtlCnt;						// 재질 개수
	bool m_bAniEnd;						// 애니메이션 종료

public:
	// 찾으려는 이름을 부모로 가지고 있는 노드를 리턴한다.
	AseNode* FindParentNodeName(const TCHAR* name);
	int FindBoneindexbyName(const TCHAR* name);

	void MakeTM(AseNode* pnode);

	void MakeTM_Fors(std::vector<AseNode*> pnodeList);

	// 애니메이션 정보가 업는 데이터가 업데이트 대신 해주면 죔
	void MakeFinalTm();
	// 제어
	int m_FirstFrame;
	int m_LasrFrame;
	int m_FrameSpeed;
	int m_TickPerFrame;

	// 돟적 시간 정보
	int m_CurrTime;
	float m_SaveTime;

	int GetCurrTime(float dTime);

public:
	int initializeModel();
	BOOL AseModelUpdate(float dTime , XMMATRIX usertm);
	void AseModelRelease();

	int ShowModelInfo();

public:
	int Generate();
};

// 모델 로드, 생성
int  ModelCreateFromASE(ID3D11Device* pDev, TCHAR* filename, AseModel** ppModel , ID3D11DeviceContext* pDevContext);

// 모델 제거.
void ModelRelease(AseModel*& ppModel);