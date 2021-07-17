//
//
//
//
// [2020/07/28 RUNSUMIN]
#pragma once
#include "vector"
#include "list"
#include "DxDefine.h"
#include "AseModel.h"
#include "AseNode.h"
#include "string"
#include "locale.h"		
//
#include "D3DDevice.h"
#include "Collision2D.h"

using namespace std;
class AseNode;
class AseModel;

#define STATIC


class AseParser
{
public:
	AseParser();
	~AseParser();

public:
	int m_LineCount;							// 총 파싱 라인 카운터
	TCHAR m_CurrLine[256];						// 읽어들인 현재라인 파싱용 임시버퍼

	FILE* m_fp;									// ASE 파일 포인터
	AseModel* m_pModelList;

public:
	int _ReadLine(TCHAR* buff);					// 한라인씩 읽을 함수
	// 애니메이션 관련
	////////////////////////////////////////////////////////////////////////////

	int _LoadTM(AseNode* pnode);



	// 매쉬 관련
	////////////////////////////////////////////////////////////////////////////
	// 최상위 객체 로드
	int _LoadGeomObject();						//지오메트리 파싱.

	// 버텍스 , 페이스 정보
	int _LoadMesh(AseNode* pnode);
	int _LoadVertexList(AseNode* pnode);
	int _LoadFaceList(AseNode* pnode);
	int	_LoadCVertexList(AseNode* pnode);
	int	_LoadCFaceList(AseNode* pnode);

	// 재질 정보
	int _LoadMaterialList();
	int _LoadMaterial();
	int _LoadMap_Reflect();
	int _LoadDiffuseMap(int count);

	// 노말 // 텍스쳐 UV
	int _LoadNormal(AseNode* pnode);
	int _LoadTexUVVertex(AseNode* pnode);
	int _LoadTexUVVFace(AseNode* pnode);

	int _LoadTexture(TCHAR* fileName, ID3D11Device* pDev, ID3D11DeviceContext* pDeviceContext);

	// 애니메이션
	int _LoadAniData(AseNode* pnode);
	int _LoadPosData(AseNode* pnode);
	int _LoadRotData(AseNode* pnode);
	int _CreateAniKeySet(AseNode* pnode);
	int _CreateAniKey();

	int _LoadSceneData();

	// 충돌
	int _LoadCollData(AseNode* pnode);

	// 스키닝
	int _LoadSkinningData(AseNode* pnode);

	// 버텍스 정보 재구성
	int _MakeVertexSkinData();

	// 버텍스 확장// Weld 전단계
	int _ExtendVertexList();

	// 노드 계충구조 만들어주기...
	int SetHeirarchy();

public:
	// 생성자 , 소멸자

	int Load(ID3D11Device* pDev, TCHAR* filename, AseModel* pModel, ID3D11DeviceContext* pDeviceContext);

	HRESULT LoadTexture(TCHAR* filename, ID3D11ShaderResourceView** ppTexRV, ID3D11Device* pDev, ID3D11DeviceContext* pDeviceContext);

	STATIC

		static void GetFileName(const TCHAR* FullPath, TCHAR* FileName);		//파일명 얻기
	static void GetPath(const TCHAR* FullPathName, TCHAR* Path);			//path 얻기

};

