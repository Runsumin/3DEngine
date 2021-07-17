//
//
//
// Ase Node 클래스 생성
// [2020/07/27 RUNSUMIN]

#pragma once

#include "DxDefine.h"
#include "D3DDevice.h"
#include <list>

typedef XMVECTOR QUATERNIONA;

// 정점 구조체 및 포멧.
struct VERTEX_ASE
{
	VECTOR3 vPos;
	COLOR   dwColor;

	// 스킨 데이터
	VECTOR4 vbWeight;
	DWORD	m_Index[4];
};

// 정점 구조체 및 포멧(Normal 추가)
struct VERTEX_ASE_NORMAL
{
	VECTOR3 vFcNormal;
	VECTOR3 vVtNormal_1;
	VECTOR3 vVtNormal_2;
	VECTOR3 vVtNormal_3;
};

// 정점 구조체(확장, FaceList에 맞춰 노말을 넣어줄 구조체) - 최종 구조체!
struct VERTEX_ASE_EXTEND
{

	VECTOR4 vbWeight;
	DWORD	m_Index[4];
	VECTOR3 vPos;
	VECTOR3 vNormal;
	VECTOR2 vUv;

};

// Face 구조체  
struct FACE_ASE
{
	DWORD A, B, C;		//32Bit..
};

// 텍스쳐 좌표 정보 구조체
struct TEX_UV
{
	VECTOR2 vVertexUV;
};

struct MATERIAL
{
	// 재질 정보
	XMFLOAT4 m_Diffuse;			// 확산광
	XMFLOAT4 m_Ambient;			// 주변광
	XMFLOAT4 m_Specular;		// 스펙큘러
	FLOAT	 m_SpcePower;		// 스펙큘러 파워
	BOOL	 m_bTexOn;			// 텍스쳐 유무...
};

struct MATERIAL_ASE
{
	// 재질 정보
	TCHAR	m_MtlTexName[256];	// 재질 정보 안 텍스쳐 파일명 
	XMFLOAT4 m_Diffuse;			// 확산광
	XMFLOAT4 m_Ambient;			// 주변광
	XMFLOAT4 m_Specular;		// 스펙큘러
	FLOAT	 m_SpcePower;		// 스펙큘러 파워

	int		m_Count;
};

struct ANIKEY_POS
{
	DWORD m_PosTickTime;
	float x, y, z;
};

struct ANIKEY_ROT
{
	DWORD m_RotTickTime;
	float x, y, z;
	float Angle;
	bool  MakeFrameZero;
};

struct KEY_POS
{
	DWORD		m_TickTime;
	VECTOR3		m_Pos;
};

struct KEY_ROT
{
	DWORD		m_TickTime;
	VECTOR3		m_RotAxis;
	float		m_Angle;
	QUATERNIONA m_RotQ;
};

struct SKIN_DATA
{
	int		m_Id;				// 카운터..
	float	m_Weight[4];		// 가중치
	DWORD	m_Index[4];			// 
	TCHAR	m_Bonename[4][256];	// 본 이름
};

class AseNode
{
	friend class AseParser;

public:
	// 메쉬 데이터.
	TCHAR	m_FileName[256];		// 파일명 
	TCHAR   m_NodeName[256];		// 노드(메쉬) 이름 
	TCHAR   m_ParentNodeName[256];	// 부모 노드(메쉬) 이름 
	UINT	m_VertexCnt;			// 정점 개수
	UINT	m_FaceCnt;				// 페이스 개수
	UINT	m_IndexCnt;				// 인덱스로 그려질 총 정점개수. 보통 Face * 3 개

	UINT	m_VertexCnt_Ex;			// 확장 버텍스 버퍼 정점개수

	int m_MtlRef;					// 몇번 재질을 사용할지...

	
	// 노드 TM
	XMFLOAT4X4	m_mTM;					// 최종 transform matrix
	XMFLOAT4X4	m_mTrans;				// 이동행렬
	XMFLOAT4X4	m_mScale;				// 크기변환 행렬
	XMFLOAT4X4	m_mRot;					// 회전행렬
	VECTOR3 m_vPos;					// 위치
	VECTOR3	m_vScale;				// 크기
	VECTOR3 m_vRot;					// 회전값

	// 기하 데이터 (임시) 버퍼.
	VERTEX_ASE_NORMAL* m_pVerNormalList;	// 노멀 저장용 리스트

	VERTEX_ASE* m_pVertList;				// 정점 리스트 : VB 용 
	FACE_ASE* m_pFaceList;					// FACE 리스트: IB 용  
	COLOR* m_pCVertList;					// 정점 색상 리스트
	FACE_ASE* m_pCFaceList;					// 정점 색상-인덱스 리스트
	TEX_UV* m_pTexUvVertexList;				// 텍스쳐 좌표 저장 리스트
	FACE_ASE* m_pTexUvFaceList;


	// 최종 버텍스
	VERTEX_ASE_EXTEND* m_pVerNrmList_Ex;	// 노멀 & 버텍스 저장용 리스트(Extend)
	FACE_ASE* m_pIndexList_EX;				// 확장용 인덱스 버퍼 저장용

	// 렌더링 (H/W) 버퍼   
	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;

	////////////////////////////////////////////////////////////////////////////////////////
	// Animation 관련
	AseNode* m_ParentNode;					// 부모 포인터 저장용
	std::vector<AseNode*> m_ChildNodelist;	// 자식 노드 저장용 리스트
	BOOL	m_bParent;						// 부모가 있는 노드인지 확인용

	// 실제 움직임
	std::vector<ANIKEY_POS> m_PosKey;		// pos 데이터 저장용
	std::vector<ANIKEY_ROT> m_RotKey;		// Rot 데이터 저장용

	// 키 개수
	int m_PosKeyCnt;							// 이동 정보 키 개수 저장용
	int m_RotKeyCnt;							// 이동 정보 키 개수 저장용

	// 키 개수가 다를 수 있어서 분리... 다루는 데이터도 다르다
	KEY_POS* m_AniPosKeyList;						// 이동 정보 키 리스트
	KEY_ROT* m_AniRotKeyList;						// 회전 정보 키 리스트

	float m_AnimSpeed;
	float m_CurrTime;
	float m_RotCurrTime;

	DWORD m_CurrTickTime;
	DWORD m_CurrKey;

	DWORD m_RotCurrTickTime;
	DWORD m_RotCurrKey;

	/////
	XMMATRIX m_TTM;

	// 애니 TM
	XMMATRIX m_mAniTM;
	XMMATRIX m_mAniTransM;
	XMMATRIX m_mAniRotM;
	XMMATRIX m_mAniScaleM;

	// 로컬 TM
	XMMATRIX m_mLocalTM;
	XMMATRIX m_mLocalTransM;
	XMMATRIX m_mLocalRotM;
	XMMATRIX m_mLocalScaleM;

	// 최종 변환 TM
	XMMATRIX m_mFinalTM;
	XMMATRIX m_mFinalTransM;
	XMMATRIX m_mFinalRotM;
	XMMATRIX m_mFinalScaleM;

	////////////////////////////////////////////////////////////////////////////////////////
	// Skinning 관련
	std::vector<SKIN_DATA> m_SkinDataList;	// 스킨 데이터 리스트

	// 예외처리
	bool m_bSkindata;	// 스킨 데이터가 있는지 없는지...
	bool m_bAniData;	// 애니메이션 데이터가 있는지 없는지
	bool m_bTexData;	// 텍스쳐가 있는지 없는지
	bool m_bSkin;
	bool m_AniEnd;		// 전체 애니메이션 루프를 다 돌았는지
public:
	int _CreateVFBuffers();			// 정점 버퍼 재구성

	HRESULT _CreateHWBuffers();		// 렌더링 버퍼 생성

	int _ExtendVertexList();		// 버텍스 구조 확장

	void PushChildNode(AseNode* pnode);

	XMMATRIX GetFinalTM();

public:
	int Generate();

public:
	//생성자 , 소멸자
	AseNode(/*LPDEVICE pDev*/);
	~AseNode();

	// 내부 정보 접근 매소드
	const XMFLOAT4X4& GetTM() const { return m_mTM; }
	const VECTOR3 GetPos() const { return m_vPos; }
	const VECTOR3 GetRot() const { return m_vRot; }
	const ID3D11Buffer* GetVB() const { return m_pVB; }
	const ID3D11Buffer* GetIB() const { return m_pIB; }
	const UINT GetVBStrideInByte() const { return sizeof(VERTEX_ASE_EXTEND); }
	const UINT GetFaceCnt() const { return m_FaceCnt; }
	const UINT GetVertexCnt() const { return m_VertexCnt; }
	const UINT GetIndexedCnt() const { return m_IndexCnt; }
	const TCHAR* GetFileName() const { return m_FileName; }
	const TCHAR* GetNodeName() const { return m_NodeName; }
	const TCHAR* GetParentNodeName() const { return m_ParentNodeName; }
	const int GetMtrlCount() const { return m_MtlRef; }
	const BOOL GetParentHave() const { return m_bParent; }
	//const XMMATRIX GetFinalTM() const { return m_mFinalTM; }

	void Release();

	XMFLOAT4X4 Xmfloat4x4Identity();
};

#ifndef EQUAL
#define EQUAL( a, b ) ( _tcsicmp((a), (b)) == 0)
#define NOT_EQUAL( a, b ) ( _tcsicmp((a), (b)) != 0)
#endif


