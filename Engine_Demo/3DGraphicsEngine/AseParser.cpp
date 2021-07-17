//
//
//
//
// [2020/07/28 RUNSUMIN]

#include "AseParser.h"


AseParser::AseParser()
{
	// 모델도 추후에 초기화
	m_pModelList = NULL;
	m_LineCount = 0;
	m_fp = NULL;
	_tcscpy(m_CurrLine, L"----");
}

AseParser::~AseParser()
{

}
// 한줄씩 읽는다
int AseParser::_ReadLine(TCHAR* buff)
{
	_tcscpy(buff, L"******");		//버퍼 클리어. 비교를위한 임시코드..
	_fgetts(buff, 256, m_fp);		//읽어들인 데이터 복사.
	m_LineCount++;					//읽어들인 라인수 증가..(디버깅용)

	return OK_SIGN;
}

// 최상위 로드 함수... 
int AseParser::Load(ID3D11Device* pDev, TCHAR* filename, AseModel* pModel, ID3D11DeviceContext* pDeviceContext)
{
	TCHAR token[256] = L"";

	assert(pDev != NULL);

	// 파일 오픈
	m_fp = _tfopen(filename, L"rt");
	if (m_fp == NULL)
	{
		return FAIL_SIGN;
	}

	_ReadLine(m_CurrLine);
	_stscanf(m_CurrLine, L"%s", token);

	if (NOT_EQUAL(token, L"*3DSMAX_ASCIIEXPORT")) return FAIL_SIGN;

	assert(pModel != NULL);
	m_pModelList = pModel;

	AseParser::GetFileName(filename, m_pModelList->m_FileName);

	while (1)
	{
		//스트림의 파일 끝을 테스트합니다.

		if (feof(m_fp)) break;

		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);		//현재줄에서 첫번째 '단어' 읽기!!

		// 버전과 만든 날짜?
		if (EQUAL(token, L"*COMMENT")) continue;

		if (EQUAL(token, L"*SCENE"))
		{
			_LoadSceneData();
		}

		if (EQUAL(token, L"*MATERIAL_LIST"))
		{
			_LoadMaterialList();
			continue;
		}

		if (EQUAL(token, L"*GEOMOBJECT"))
		{
			_LoadGeomObject();
			continue;
		}
	}
	fclose(m_fp);

	// 스키닝 정보 저장
	_MakeVertexSkinData();

	// 확장 버텍스 버퍼 설정
	_ExtendVertexList();

	// 텍스쳐 정보 불러오기
	_LoadTexture(filename, pDev, pDeviceContext);

	// 계층구조 설정
	SetHeirarchy();

	// 애니메이션 키 설정
	_CreateAniKey();


	return OK_SIGN;
}

HRESULT AseParser::LoadTexture(TCHAR* filename, ID3D11ShaderResourceView** ppTexRV, ID3D11Device* pDev, ID3D11DeviceContext* pDeviceContext)
{
	HRESULT hr = S_OK;

	ID3D11ShaderResourceView* pTexRV = nullptr;

	//TCHAR* fileName = L"../data/sonim.jpg";

	//일반 텍스처 로드.
	//hr = DirectX::CreateWICTextureFromFile( g_pDevice, fileName, nullptr, &  );	

	//밉멥+텍스처 로드.
	//hr = DirectX::CreateWICTextureFromFileEx(pDev, pDeviceContext, filename, 0,
	//	D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
	//	0, D3D11_RESOURCE_MISC_GENERATE_MIPS, WIC_LOADER_DEFAULT,
	//	nullptr, &pTexRV);

	hr = DirectX::CreateWICTextureFromFile(pDev, filename, nullptr, &pTexRV);

	if (FAILED(hr))
	{
		//DDS 파일로 로드 시도. : +밉멥
	/*	hr = DirectX::CreateDDSTextureFromFileEx( g_pDevice, g_pDXDC, filename, 0,
				D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, //| D3D11_BIND_RENDER_TARGET,
				0,	D3D11_RESOURCE_MISC_GENERATE_MIPS,	false, //_LOADER_DEFAULT,
				nullptr, &pTexRV );	 */
		hr = DirectX::CreateDDSTextureFromFile(pDev, filename, nullptr, &pTexRV);
		if (FAILED(hr))
		{
			return hr;
		}
	}

	//외부로 리턴.
	*ppTexRV = pTexRV;

	return hr;
}

int AseParser::_LoadGeomObject()
{
	TCHAR token[256] = L"";

	AseNode* tnode = new AseNode();
	assert(tnode != NULL);

	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		// switch 로 바꾸자

		// 노드 이름..
		if (EQUAL(token, L"*NODE_NAME"))
		{
			_stscanf(m_CurrLine, L"\t *NODE_NAME \"%[^\"]\"", &tnode->m_NodeName);
			continue;
		}

		// 부모 노드 이름..
		if (EQUAL(token, L"*NODE_PARENT"))
		{
			_stscanf(m_CurrLine, L"\t *NODE_PARENT \"%[^\"]\"", &tnode->m_ParentNodeName);
			continue;
		}

		//노드 행렬.. 
		if (EQUAL(token, L"*NODE_TM"))
		{
			_LoadTM(tnode);
			continue;
		}

		//메쉬 정보..
		if (EQUAL(token, L"*MESH"))
		{
			_LoadMesh(tnode);
			continue;
		}

		// 만약 충돌 관련된 정보라면 이부분을 탄다
		if (EQUAL(token, L"*SHAPE_LINE"))
		{
			_LoadCollData(tnode);
			continue;
		}

		// 애니메이션 정보
		if (EQUAL(token, L"*TM_ANIMATION"))
		{
			_LoadAniData(tnode);
			continue;
		}

		// 스킨정보
		if (EQUAL(token, L"*SKIN"))
		{
			tnode->m_bSkin = true;
			_LoadSkinningData(tnode);
		}

		if (EQUAL(token, L"*MATERIAL_REF"))
		{
			tnode->m_bTexData = true;
			_stscanf(m_CurrLine, L"\t *MATERIAL_REF %d", &tnode->m_MtlRef);
		}

		// 추후에 추가되는 데이터들 로드해주자

		if (EQUAL(token, L"}")) break;
	}

	// 본 정보면 본노드 리스트에 추가(노드 네임으로 판별)
	if (_tcsstr(tnode->m_NodeName, L"Bip") != NULL ||
		_tcsstr(tnode->m_NodeName, L"Bone") != NULL)
	{
		m_pModelList->m_BoneList.push_back(tnode);
	}
	// 맵 전용 충돌 처리...
	/*else if (_tcsstr(tnode->m_NodeName, L"Weapon") != NULL)
	{
		m_pModelList->m_Weapon = tnode;
	}*/
	// 플레이어 무기 노드
	else if (_tcsstr(tnode->m_NodeName, L"CollCheck") != NULL)
	{
		m_pModelList->m_CollList.push_back(tnode);
	}

	else
	{		// 메쉬 정보면 메쉬노드 리스트에 추가
		m_pModelList->m_NodeList.push_back(tnode);
	}

	return OK_SIGN;
}

int AseParser::_LoadTM(AseNode* pnode)
{
	TCHAR token[256] = L"";
	int num = 0;

	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*TM_ROW0"))
		{
			// 부모 matrix
			_stscanf(m_CurrLine, L"\t *TM_ROW %d %f %f %f", &num,
				&pnode->m_mTM._11,
				&pnode->m_mTM._13,
				&pnode->m_mTM._12);
		}

		if (EQUAL(token, L"*TM_ROW1"))
		{
			// 부모 matrix
			_stscanf(m_CurrLine, L"\t *TM_ROW %d %f %f %f", &num,
				&pnode->m_mTM._31,
				&pnode->m_mTM._33,
				&pnode->m_mTM._32);
		}

		if (EQUAL(token, L"*TM_ROW2"))
		{
			// 부모 matrix
			_stscanf(m_CurrLine, L"\t *TM_ROW %d %f %f %f", &num,
				&pnode->m_mTM._21,
				&pnode->m_mTM._23,
				&pnode->m_mTM._22);
		}

		if (EQUAL(token, L"*TM_ROW3"))
		{
			// 부모 matrix
			_stscanf(m_CurrLine, L"\t *TM_ROW %d %f %f %f", &num,
				&pnode->m_mTM._41,
				&pnode->m_mTM._43,
				&pnode->m_mTM._42);
		}

		if (EQUAL(token, L"*TM_POS"))
		{
			// 부모 matrix
			_stscanf(m_CurrLine, L"\t *TM_POS %f %f %f",
				&pnode->m_vPos.x,
				&pnode->m_vPos.z,
				&pnode->m_vPos.y);
		}

		if (EQUAL(token, L"*TM_ROTAXIS"))
		{
			// 부모 matrix
			_stscanf(m_CurrLine, L"\t *TM_ROTAXIS %f %f %f",
				&pnode->m_vRot.x,
				&pnode->m_vRot.z,
				&pnode->m_vRot.y);
		}

		if (EQUAL(token, L"*TM_SCALE"))
		{
			// 부모 matrix
			_stscanf(m_CurrLine, L"\t *TM_SCALE %f %f %f",
				&pnode->m_vScale.x,
				&pnode->m_vScale.z,
				&pnode->m_vScale.y);
		}

		//닫기.
		if (EQUAL(token, L"}")) break;
	}

	return OK_SIGN;
}

int AseParser::_LoadMesh(AseNode* pnode)
{
	TCHAR token[256] = L"";
	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		// 버텍스 정보(개수)
		if (EQUAL(token, L"*MESH_NUMVERTEX"))
		{
			_stscanf(m_CurrLine, L"\t *MESH_NUMVERTEX %d", &pnode->m_VertexCnt);
			continue;
		}

		// 페이스 정보
		if (EQUAL(token, L"*MESH_NUMFACES"))
		{
			_stscanf(m_CurrLine, L"\t *MESH_NUMFACES %d", &pnode->m_FaceCnt);
			continue;
		}

		// 각 버텍스들의 위치정보(Local)
		if (EQUAL(token, L"*MESH_VERTEX_LIST"))
		{
			_LoadVertexList(pnode);
			continue;
		}

		//'페이스별 정점 리스트 (Index Buffer) 정보 로드.
		if (EQUAL(token, L"*MESH_FACE_LIST"))
		{
			_LoadFaceList(pnode);
			continue;
		}
		//정점색 리스트..
		if (EQUAL(token, L"*MESH_NUMCVERTEX"))
		{
			_LoadCVertexList(pnode);
			continue;
		}

		//'페이스별 정점색 인덱스 리스트' 로드.
		if (EQUAL(token, L"*MESH_NUMCVFACES"))
		{
			_LoadCFaceList(pnode);
			continue;
		}

		// 버텍스별 텍스쳐 좌표 로드
		if (EQUAL(token, L"*MESH_NUMTVERTEX"))
		{
			_LoadTexUVVertex(pnode);
			continue;
		}

		// 페이스별 텍스쳐 좌표 로드
		if (EQUAL(token, L"*MESH_NUMTVFACES"))
		{
			_LoadTexUVVFace(pnode);
			continue;
		}

		// 페이스별 노멀 정보 로드
		if (EQUAL(token, L"*MESH_NORMALS"))
		{
			_LoadNormal(pnode);
			continue;
		}

		// 종료
		if (EQUAL(token, L"}")) break;
	}
	return OK_SIGN;
}

int AseParser::_LoadVertexList(AseNode* pnode)
{

	TCHAR token[256] = L"";
	UINT count = 0;
	UINT number = 0;

	pnode->m_pVertList = new VERTEX_ASE[pnode->m_VertexCnt];
	assert(pnode->m_pVertList != NULL);

	// 정점 개수만큼 읽어야 한다...
	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*MESH_VERTEX"))
		{
			VERTEX_ASE temp;

			_stscanf(m_CurrLine, L"\t *MESH_VERTEX %d %f %f %f",
				&number,
				&temp.vPos.x,
				&temp.vPos.z,
				&temp.vPos.y);


			XMVECTOR vec = XMLoadFloat3(&temp.vPos);
			XMMATRIX _store = XMLoadFloat4x4(&pnode->m_mTM);
			XMMATRIX _Inv = XMMatrixInverse(nullptr, _store);
			vec = XMVector3Transform(vec, _Inv);
			XMStoreFloat3(&pnode->m_pVertList[count].vPos, vec);

			count++;
			continue;
		}

		// } 문자가 나오면 중단
		if (EQUAL(token, L"}"))
			break;
	}
	// 오류는 추후에 추가

	//오류 검증 :  정점 개수만큼 루프를 돌아야 합니다.
	if (count != pnode->m_VertexCnt)
	{
		//error...
		return FAIL_SIGN;
	}

	return OK_SIGN;
}

// 인덱스 버퍼 정보 로드
int AseParser::_LoadFaceList(AseNode* pnode)
{
	TCHAR token[256] = L"";
	UINT count = 0;
	UINT number = 0;

	pnode->m_pFaceList = new FACE_ASE[pnode->m_FaceCnt];
	assert(pnode->m_pFaceList != NULL);
	//ZeroMemory(pnode->m_pFaceList, sizeof(FACE_ASE) * pnode->m_FaceCnt);

	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		//
		if (EQUAL(token, L"*MESH_FACE"))
		{
			// 한 줄을 읽을때 z 와 y 의 위치 바꿔준다
			//*MESH_FACE    0:    A:    0 B:    3 C:
			_stscanf(m_CurrLine, L"\t *MESH_FACE %d:  A: %d  B: %d  C: %d",
				&number,
				&pnode->m_pFaceList[count].A,
				&pnode->m_pFaceList[count].C,
				&pnode->m_pFaceList[count].B);

			count++;
			continue;
		}

		// } 문자가 나오면 중단
		if (EQUAL(token, L"}"))
			break;

	}


	// 오류 출력은 추후에... 빠르게 작업 후 정확히 동작하는지 확인하자!

	//오류 검증 :  삼각형 개수만큼 읽어야함
	if (count != pnode->m_FaceCnt)
	{
		//error...
		return FAIL_SIGN;
	}

	return OK_SIGN;
}
// 각 버텍스의 diffuse color load
int AseParser::_LoadCVertexList(AseNode* pnode)
{
	TCHAR token[256] = L"";
	int count;

	_stscanf(m_CurrLine, L"\t *MESH_NUMCVERTEX %d", &count);
	if (count <= 0) return FAIL_SIGN;

	pnode->m_pCVertList = new COLOR[count];
	assert(pnode->m_pCVertList != NULL);
	//ZeroMemory(pnode->m_pCVertList, sizeof(COLOR) * count);

	int number;
	COLOR color(1, 1, 1, 1);

	_ReadLine(m_CurrLine);

	for (int i = 0; i < count; i++)
	{
		//*MESH_VERTCOL 0	0.9619	0.9964	0.0198
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"\t *MESH_VERTCOL %d %f %f %f", &number, &color.x, &color.y, &color.z);
		pnode->m_pCVertList[number] = color;
	}

	// 한줄 읽어주기...
	_ReadLine(m_CurrLine);
	return OK_SIGN;
}

int AseParser::_LoadCFaceList(AseNode* pnode)
{
	TCHAR token[256] = L"";
	int count = 0;

	_stscanf(m_CurrLine, L"\t *MESH_NUMCVFACES %d", &count);
	if (count <= 0) return FAIL_SIGN;

	pnode->m_pCFaceList = new FACE_ASE[count];
	assert(pnode->m_pCFaceList != NULL);

	if (pnode->m_pCFaceList == NULL)
	{
		//error..!!
		return FAIL_SIGN;
	}

	_ReadLine(m_CurrLine);
	int number = 0;
	for (int i = 0; i < count; i++)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"\t *MESH_CFACE %d %d %d %d", &number,
			&pnode->m_pCFaceList[number].A,
			&pnode->m_pCFaceList[number].C,
			&pnode->m_pCFaceList[number].B);
	}

	// 한줄 읽어주기...
	_ReadLine(m_CurrLine);
	return OK_SIGN;
}


int AseParser::_LoadTexUVVertex(AseNode* pnode)
{
	TCHAR token[256] = L"";
	int count = 0;

	_stscanf(m_CurrLine, L"\t *MESH_NUMTVERTEX %d", &count);
	if (count <= 0) return FAIL_SIGN;

	pnode->m_pTexUvVertexList = new TEX_UV[count];
	assert(pnode->m_pTexUvVertexList != NULL);

	_ReadLine(m_CurrLine);
	int num = 0;
	for (int i = 0; i < count; i++)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"\t *MESH_TVERT %d %f %f", &num,
			&pnode->m_pTexUvVertexList[i].vVertexUV.x,
			&pnode->m_pTexUvVertexList[i].vVertexUV.y);

		pnode->m_pTexUvVertexList[i].vVertexUV.y = 1 - pnode->m_pTexUvVertexList[i].vVertexUV.y;
	}

	_ReadLine(m_CurrLine);

	return OK_SIGN;

}

int AseParser::_LoadTexUVVFace(AseNode* pnode)
{
	TCHAR token[256] = L"";
	int count = 0;

	_stscanf(m_CurrLine, L"\t *MESH_NUMTVFACES %d", &count);
	if (count <= 0) return FAIL_SIGN;

	pnode->m_pTexUvFaceList = new FACE_ASE[count];
	assert(pnode->m_pTexUvFaceList != NULL);

	_ReadLine(m_CurrLine);
	int num = 0;
	for (int i = 0; i < count; i++)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"\t *MESH_TFACE %d  %d %d %d",
			&num,
			&pnode->m_pTexUvFaceList[i].A,
			&pnode->m_pTexUvFaceList[i].C,
			&pnode->m_pTexUvFaceList[i].B);
	}
	_ReadLine(m_CurrLine);

	return OK_SIGN;
}

int AseParser::_LoadNormal(AseNode* pnode)
{
	TCHAR token[256] = L"";
	int count = pnode->m_FaceCnt;

	pnode->m_pVerNormalList = new VERTEX_ASE_NORMAL[pnode->m_FaceCnt];


	// 공간 채워줄 임시 변수
	int number = 0;
	int Dumpnum = 0;
	for (int i = 0; i < count; i++)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		_stscanf(m_CurrLine, L"\t *MESH_FACENORMAL %d %f %f %f",
			&number,
			&pnode->m_pVerNormalList[i].vFcNormal.x,
			&pnode->m_pVerNormalList[i].vFcNormal.z,
			&pnode->m_pVerNormalList[i].vFcNormal.y);

		_ReadLine(m_CurrLine);

		_stscanf(m_CurrLine, L"\t *MESH_VERTEXNORMAL %d %f %f %f",
			&Dumpnum,
			&pnode->m_pVerNormalList[i].vVtNormal_1.x,
			&pnode->m_pVerNormalList[i].vVtNormal_1.z,
			&pnode->m_pVerNormalList[i].vVtNormal_1.y);

		_ReadLine(m_CurrLine);

		_stscanf(m_CurrLine, L"\t *MESH_VERTEXNORMAL %d %f %f %f",
			&Dumpnum,
			&pnode->m_pVerNormalList[i].vVtNormal_2.x,
			&pnode->m_pVerNormalList[i].vVtNormal_2.z,
			&pnode->m_pVerNormalList[i].vVtNormal_2.y);

		_ReadLine(m_CurrLine);

		_stscanf(m_CurrLine, L"\t *MESH_VERTEXNORMAL %d %f %f %f",
			&Dumpnum,
			&pnode->m_pVerNormalList[i].vVtNormal_3.x,
			&pnode->m_pVerNormalList[i].vVtNormal_3.z,
			&pnode->m_pVerNormalList[i].vVtNormal_3.y);
	}

	//_ReadLine(m_CurrLine);
	_ReadLine(m_CurrLine);
	return OK_SIGN;
}

int AseParser::_ExtendVertexList()
{
	int cnt = m_pModelList->m_NodeList.size();

	for (int i = 0; i < cnt; i++)
	{
		m_pModelList->m_NodeList[i]->_ExtendVertexList();
	}

	cnt = m_pModelList->m_BoneList.size();

	for (int i = 0; i < cnt; i++)
	{
		m_pModelList->m_BoneList[i]->_ExtendVertexList();
	}

	cnt = m_pModelList->m_CollList.size();

	for (int i = 0; i < cnt; i++)
	{
		m_pModelList->m_CollList[i]->_ExtendVertexList();
	}

	return OK_SIGN;

}

int AseParser::SetHeirarchy()
{
	int boneCnt = m_pModelList->m_BoneList.size();

	for (int i = 0; i < boneCnt; i++)
	{
		// 부모 이름 받아오기
		TCHAR* parentName = m_pModelList->m_BoneList[i]->m_ParentNodeName;

		AseNode* parentNode = nullptr;

		// 만약 부모 노드가 없다면...?
		if (EQUAL(parentName, L"NA"))
		{
			m_pModelList->m_RootNodeList.push_back(m_pModelList->m_BoneList[i]);
			m_pModelList->m_RootNode = m_pModelList->m_BoneList[i];
			m_pModelList->m_BoneList[i]->m_ParentNode = nullptr;
			//continue;
		}
		else
		{
			parentNode = m_pModelList->FindParentNodeName(parentName);

			m_pModelList->m_BoneList[i]->m_ParentNode = parentNode;

			parentNode->PushChildNode(m_pModelList->m_BoneList[i]);

		}
	}


	int nodeCnt = m_pModelList->m_NodeList.size();

	for (int i = 0; i < nodeCnt; i++)
	{
		// 부모 이름 받아오기
		TCHAR* parentName = m_pModelList->m_NodeList[i]->m_ParentNodeName;

		AseNode* parentNode = nullptr;

		// 만약 부모 노드가 없다면...?
		if (EQUAL(parentName, L"NA"))
		{
			m_pModelList->m_RootNodeList.push_back(m_pModelList->m_NodeList[i]);

			m_pModelList->m_RootNode = m_pModelList->m_NodeList[i];
			m_pModelList->m_NodeList[i]->m_ParentNode = nullptr;
			//continue;
		}
		else
		{
			parentNode = m_pModelList->FindParentNodeName(parentName);

			m_pModelList->m_NodeList[i]->m_ParentNode = parentNode;

			parentNode->PushChildNode(m_pModelList->m_NodeList[i]);

		}
	}

	return OK_SIGN;
}

int AseParser::_LoadMaterialList()
{
	TCHAR token[256] = L"";

	// 재질의 총 개수 저장
	_ReadLine(m_CurrLine);
	_stscanf(m_CurrLine, L"%s", token);



	if (EQUAL(token, L"*MATERIAL_COUNT"))
	{
		_stscanf(m_CurrLine, L"\t *MATERIAL_COUNT %d", &m_pModelList->m_MtlCnt);
	}

	UINT cnt = m_pModelList->m_MtlCnt;

	m_pModelList->m_ModelMtlList = new MODEL_MATERIAL[cnt];
	assert(m_pModelList->m_ModelMtlList != NULL);
	ZeroMemory(m_pModelList->m_ModelMtlList, sizeof(MODEL_MATERIAL) * cnt);

	for (int i = 0; i < cnt; i++)
	{
		m_pModelList->m_ModelMtlList[i].m_bTexOn = FALSE;
	}

	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*MATERIAL"))
		{
			_LoadMaterial();
			continue;
		}

		// 닫기 }
		if (EQUAL(token, L"}")) break;
	}

	return OK_SIGN;

}

int AseParser::_LoadMaterial()
{
	TCHAR token[256] = L"";
	int count = 0;

	// 재질 번호.... 카운트로 사용하자
	_stscanf(m_CurrLine, L"\t *MATERIAL %d", &count);

	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*MATERIAL_AMBIENT"))
		{
			_stscanf(m_CurrLine, L"\t *MATERIAL_AMBIENT %f %f %f",
				&m_pModelList->m_ModelMtlList[count].m_Ambient.x,
				&m_pModelList->m_ModelMtlList[count].m_Ambient.y,
				&m_pModelList->m_ModelMtlList[count].m_Ambient.z);
		}
		if (EQUAL(token, L"*MATERIAL_DIFFUSE"))
		{
			_stscanf(m_CurrLine, L"\t *MATERIAL_DIFFUSE %f %f %f",
				&m_pModelList->m_ModelMtlList[count].m_Diffuse.x,
				&m_pModelList->m_ModelMtlList[count].m_Diffuse.y,
				&m_pModelList->m_ModelMtlList[count].m_Diffuse.z);
		}
		if (EQUAL(token, L"*MATERIAL_SPECULAR"))
		{
			_stscanf(m_CurrLine, L"\t *MATERIAL_SPECULAR %f %f %f",
				&m_pModelList->m_ModelMtlList[count].m_Specular.x,
				&m_pModelList->m_ModelMtlList[count].m_Specular.y,
				&m_pModelList->m_ModelMtlList[count].m_Specular.z);
		}
		if (EQUAL(token, L"*MATERIAL_SHINE"))
		{
			_stscanf(m_CurrLine, L"\t *MATERIAL_SHINE %f", &m_pModelList->m_ModelMtlList[count].m_SpcePower);
		}

		if (EQUAL(token, L"*MAP_DIFFUSE"))
		{
			_LoadDiffuseMap(count);
			continue;
		}

		if (EQUAL(token, L"*MAP_REFLECT"))
		{
			_LoadMap_Reflect();
			_ReadLine(m_CurrLine);
			break;
		}

		if (EQUAL(token, L"}"))
			break;
	}
	return OK_SIGN;
}

int AseParser::_LoadMap_Reflect()
{
	TCHAR token[256] = L"";

	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		// 사용하지 않는 정보들... 넘겨주자
		if (EQUAL(token, L"}"))
		{
			//_ReadLine(m_CurrLine);
			break;
		}
	}

	return OK_SIGN;
}

int AseParser::_LoadDiffuseMap(int count)
{
	TCHAR token[256] = L"";

	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*BITMAP"))
		{
			TCHAR TexName[256] = L"";
			_stscanf(m_CurrLine, L"\t *BITMAP \"%[^\"]\"", &TexName);
			GetFileName(TexName, m_pModelList->m_ModelMtlList[count].m_MtlTexName);
			m_pModelList->m_ModelMtlList[count].m_bTexOn = TRUE;
		}

		if (EQUAL(token, L"}"))
		{
			break;
		}

	}


	return OK_SIGN;
}


int AseParser::_LoadTexture(TCHAR* fileName, ID3D11Device* pDev, ID3D11DeviceContext* pDeviceContext)
{
	// 한폴더 안에 있다는 가정 하에....
	TCHAR path[256] = L"";
	GetPath(fileName, path);

	int mtlCount = m_pModelList->m_MtlCnt;
	if (mtlCount == 0)	return OK_SIGN;

	TCHAR texFilename[256] = L"";
	for (int i = 0; i < mtlCount; i++)
	{
		MODEL_MATERIAL* pTexture = &m_pModelList->m_ModelMtlList[i];

		int num = _tcslen(pTexture->m_MtlTexName);
		if (num == 0)
			continue;

		//// 비트맵 로드
		_tcscpy(texFilename, path);
		_tcscat(texFilename, pTexture->m_MtlTexName);
		LoadTexture(texFilename, &pTexture->m_pTexture, pDev, pDeviceContext);

		m_pModelList->m_ModelMtlList[i].m_pTexture = pTexture->m_pTexture;
	}

	return OK_SIGN;
}


int AseParser::_LoadAniData(AseNode* pnode)
{
	TCHAR token[256] = L"";

	int num = 0;

	pnode->m_bAniData = true;

	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*CONTROL_POS_TRACK"))
		{
			_LoadPosData(pnode);
		}

		if (EQUAL(token, L"*CONTROL_ROT_TRACK"))
		{
			_LoadRotData(pnode);
		}

		if (EQUAL(token, L"}"))
			break;

	}
	return OK_SIGN;
}

int AseParser::_LoadPosData(AseNode* pnode)
{
	TCHAR token[256] = L"";
	int num = 0;
	ANIKEY_POS PosData;

	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*CONTROL_POS_SAMPLE"))
		{
			_stscanf(m_CurrLine, L"\t *CONTROL_POS_SAMPLE %d %f %f %f",
				&PosData.m_PosTickTime,
				&PosData.x,
				&PosData.z,
				&PosData.y);

			if (num == 0 && PosData.m_PosTickTime != 0)
			{
				ANIKEY_POS FirstKey;
				FirstKey.m_PosTickTime = 0;
				FirstKey.x = PosData.x;
				FirstKey.y = PosData.y;
				FirstKey.z = PosData.z;
				pnode->m_PosKey.push_back(FirstKey);
				num++;

			}

			pnode->m_PosKey.push_back(PosData);
			num++;
			continue;
		}

		if (EQUAL(token, L"}"))
		{
			// 키 개수(이동정보)
			pnode->m_PosKeyCnt = pnode->m_PosKey.size();
			break;
		}

	}

	return OK_SIGN;

}

int AseParser::_LoadRotData(AseNode* pnode)
{
	TCHAR token[256] = L"";
	int num = 0;
	ANIKEY_ROT RotData;

	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*CONTROL_ROT_SAMPLE"))
		{
			_stscanf(m_CurrLine, L"\t *CONTROL_ROT_SAMPLE %d %f %f %f %f",
				&RotData.m_RotTickTime,
				&RotData.x,
				&RotData.z,
				&RotData.y,
				&RotData.Angle);

			RotData.MakeFrameZero = false;

			if (num == 0 && RotData.m_RotTickTime != 0)
			{
				// 키프레임이 여기를 통과했다면...
				ANIKEY_ROT FirstKey;
				FirstKey.m_RotTickTime = 0;
				FirstKey.x = RotData.x;
				FirstKey.y = RotData.y;
				FirstKey.z = RotData.z;
				FirstKey.Angle = RotData.Angle;
				FirstKey.MakeFrameZero = true;
				pnode->m_RotKey.push_back(FirstKey);
				num++;

				RotData.MakeFrameZero = true;
			}

			pnode->m_RotKey.push_back(RotData);
			num++;
			continue;
		}

		if (EQUAL(token, L"}"))
		{
			// 키 개수(회전 정보)
			pnode->m_RotKeyCnt = pnode->m_RotKey.size();
			break;
		}

	}

	return OK_SIGN;
}

// 최상위 루트 노드를 셋팅해주자
int AseParser::_CreateAniKeySet(AseNode* pnode)
{
	// 이동은 있어도 회전은 없을 수 있고 반대의 경우도 가능

	int poskeycnt = pnode->m_PosKeyCnt;
	// 이동정보 키 개수가 0 이라는건 애니메이션 정보가 없다는 뜻
	if (poskeycnt > 0)
	{
		pnode->m_AniPosKeyList = new KEY_POS[poskeycnt];

		for (int i = 0; i < poskeycnt; i++)
		{
			ANIKEY_POS posdata = pnode->m_PosKey[i];
			pnode->m_AniPosKeyList[i].m_TickTime = posdata.m_PosTickTime;
			pnode->m_AniPosKeyList[i].m_Pos = VECTOR3(posdata.x, posdata.y, posdata.z);
		}

	}

	int RotKeycnt = pnode->m_RotKeyCnt;
	// 역시 0이면 회전 애니메이션 없다는 뜻
	if (RotKeycnt > 0)
	{
		pnode->m_AniRotKeyList = new KEY_ROT[RotKeycnt];

		for (int i = 0; i < RotKeycnt; i++)
		{
			ANIKEY_ROT rotdata = pnode->m_RotKey[i];
			pnode->m_AniRotKeyList[i].m_TickTime = rotdata.m_RotTickTime;
			pnode->m_AniRotKeyList[i].m_RotAxis = VECTOR3(rotdata.x, rotdata.y, rotdata.z);
			pnode->m_AniRotKeyList[i].m_Angle = rotdata.Angle;


			//	VECTOR3 AxisData = VECTOR3(rotdata.x, rotdata.y, rotdata.z);
			VECTOR3 AxisData = pnode->m_AniRotKeyList[i].m_RotAxis;

			// 회전 키 구면보간
			VECTOR axis = XMLoadFloat3(&AxisData);
			QUATERNIONA q = XMQuaternionRotationAxis(axis, pnode->m_AniRotKeyList[i].m_Angle);

			if (i == 0)
			{
				pnode->m_AniRotKeyList[i].m_RotQ = q;						// 1번째 키는 "절대값" 으로 판정가능 : 바로 저장.
			}
			else
			{
				if (i == 1 && rotdata.MakeFrameZero == true)
				{
					pnode->m_AniRotKeyList[i].m_RotQ = q;						// 1번째 키는 "절대값" 으로 판정가능 : 바로 저장.
				}
				else
				{
					pnode->m_AniRotKeyList[i].m_RotQ = XMQuaternionMultiply(pnode->m_AniRotKeyList[i - 1].m_RotQ, q);	//2번째 키부터는 이전 키와 누계, 절대 쿼터니온으로 환산.
				}
			}
		}

	}

	int cnt = pnode->m_ChildNodelist.size();
	if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			_CreateAniKeySet(pnode->m_ChildNodelist[i]);
		}
	}
	return OK_SIGN;
}

int AseParser::_CreateAniKey()
{
	_CreateAniKeySet(m_pModelList->m_RootNode);

	for (int i = 0; i < m_pModelList->m_RootNodeList.size(); i++)
	{
		_CreateAniKeySet(m_pModelList->m_RootNodeList[i]);
	}
	return OK_SIGN;
}

int AseParser::_LoadSceneData()
{
	TCHAR token[256] = L"";

	while (1)
	{
		_ReadLine(m_CurrLine);
		_stscanf(m_CurrLine, L"%s", token);

		if (EQUAL(token, L"*SCENE_FIRSTFRAME"))
		{
			_stscanf(m_CurrLine, L"\t *SCENE_FIRSTFRAME %d", &m_pModelList->m_FirstFrame);
		}
		
		if (EQUAL(token, L"*SCENE_LASTFRAME"))
		{
			_stscanf(m_CurrLine, L"\t *SCENE_LASTFRAME %d", &m_pModelList->m_LasrFrame);
		}

		if (EQUAL(token, L"*SCENE_FRAMESPEED"))
		{
			_stscanf(m_CurrLine, L"\t *SCENE_FRAMESPEED %d", &m_pModelList->m_FrameSpeed);
		}

		if (EQUAL(token, L"*SCENE_TICKSPERFRAME"))
		{
			_stscanf(m_CurrLine, L"\t *SCENE_TICKSPERFRAME %d", &m_pModelList->m_TickPerFrame);
		}

		if (EQUAL(token, L"}"))
			break;
	}
	return OK_SIGN;
}

int AseParser::_LoadCollData(AseNode* pnode)
{
	return OK_SIGN;
}

int AseParser::_LoadSkinningData(AseNode* pnode)
{
	TCHAR token[256] = L"";
	int num = 0;
	bool bonce = false;
	pnode->m_bSkindata = true;

	while (1)
	{
		if (EQUAL(token, L"}"))
			break;

		SKIN_DATA tempskindata;
		ZeroMemory(&tempskindata, sizeof(SKIN_DATA));

		// 맨 처음 한번만 처리 해주는 것
		if (bonce == false)
		{
			bonce = true;
			_ReadLine(m_CurrLine);
			_stscanf(m_CurrLine, L"%s", token);
		}


		if (EQUAL(token, L"*VERTEX"))
		{
			_stscanf(m_CurrLine, L"\t *VERTEX ID: %d W: %d",
				&tempskindata.m_Id,
				&num);

			_ReadLine(m_CurrLine);
			_stscanf(m_CurrLine, L"%s", token);

			// 카운트에 n개가 들어와도 하나면 리드라인 X
			for (int i = 0; i < num; i++)
			{
				if (EQUAL(token, L"*WEIGHT"))
				{
					_stscanf(m_CurrLine, L"\t *WEIGHT %d %f BONE: \"%[^\"]\"",
						&tempskindata.m_Index[i],
						&tempskindata.m_Weight[i],
						&tempskindata.m_Bonename[i]);

					_ReadLine(m_CurrLine);
					_stscanf(m_CurrLine, L"%s", token);
				}	
				else
				{
					// 그냥 넘어간다
				}
				// 마지막일때
				if (i == num - 1)
				{
					pnode->m_SkinDataList.push_back(tempskindata);
				}

			}
			continue;
		}
	}


	return OK_SIGN;
}

int AseParser::_MakeVertexSkinData()
{

	// 노드 & 본 - 노드 ->  스킨
	for (int i = 0; i < m_pModelList->m_NodeList.size(); i++)
	{
		// 스킨 데이터가 있을 때만 값 전달...
		if (m_pModelList->m_NodeList[i]->m_SkinDataList.size() > 0)
		{
			AseNode* tmpnode = m_pModelList->m_NodeList[i];

			int Skindatacnt = tmpnode->m_VertexCnt;

			for (int j = 0; j < Skindatacnt; j++)
			{
				tmpnode->m_pVertList[j].vbWeight.x = m_pModelList->m_NodeList[i]->m_SkinDataList[j].m_Weight[0];
				tmpnode->m_pVertList[j].vbWeight.y = m_pModelList->m_NodeList[i]->m_SkinDataList[j].m_Weight[1];
				tmpnode->m_pVertList[j].vbWeight.z = m_pModelList->m_NodeList[i]->m_SkinDataList[j].m_Weight[2];
				tmpnode->m_pVertList[j].vbWeight.w = m_pModelList->m_NodeList[i]->m_SkinDataList[j].m_Weight[3];

				tmpnode->m_pVertList[j].m_Index[0] = m_pModelList->FindBoneindexbyName(m_pModelList->m_NodeList[i]->m_SkinDataList[j].m_Bonename[0]);
				tmpnode->m_pVertList[j].m_Index[1] = m_pModelList->FindBoneindexbyName(m_pModelList->m_NodeList[i]->m_SkinDataList[j].m_Bonename[1]);
				tmpnode->m_pVertList[j].m_Index[2] = m_pModelList->FindBoneindexbyName(m_pModelList->m_NodeList[i]->m_SkinDataList[j].m_Bonename[2]);
				tmpnode->m_pVertList[j].m_Index[3] = m_pModelList->FindBoneindexbyName(m_pModelList->m_NodeList[i]->m_SkinDataList[j].m_Bonename[3]);
			}
		}
	}
	return OK_SIGN;
}

// 파일 경로에서 실제 파일명만 얻어내는 함수
void AseParser::GetFileName(const TCHAR* FullPath, TCHAR* FileName)
{
	std::wstring name;		// 파일명 저장용 임시변수
	name = FullPath;
	size_t pos = name.find_last_of('\\');
	if (pos <= 0)
	{
		pos = name.find_last_of('/');
	}
	_tcscpy(FileName, &name[pos + 1]);
}

void AseParser::GetPath(const TCHAR* FullPathName, TCHAR* Path)
{
	std::wstring path;		// 경로 저장용 임시 변수
	path = FullPathName;
	size_t pos = path.find_last_of('/');
	if (pos <= 0)
	{
		pos = path.find_last_of('\\');
	}
	_tcsncpy(Path, FullPathName, pos + 1);
}