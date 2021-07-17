//
//
// 
//
// [2020/06/23 RUNSUMIN]
#include "Shader.h"

#include "AseParser.h"
#include "string"
#include "AseNode.h"

#include "ModelObject.h"

AseNode::AseNode(/*LPDEVICE pDev*/)
{
	// 모델 생성시 초기화
	_tcscpy(m_FileName, L"NA");
	_tcscpy(m_NodeName, L"NA");
	_tcscpy(m_ParentNodeName, L"NA");

	m_VertexCnt = 0;
	m_FaceCnt = 0;
	m_IndexCnt = 0;
	m_VertexCnt_Ex = 0;
	m_MtlRef = 0;

	// 노드 TM
	m_mTM = Xmfloat4x4Identity();
	m_mTrans = m_mScale = m_mRot = m_mTM;

	m_mAniTM = XMMatrixIdentity();
	m_mAniTransM = m_mAniRotM = m_mAniScaleM = m_mAniTM;

	m_mLocalTM = XMMatrixIdentity();
	m_mLocalTransM = m_mLocalRotM = m_mLocalScaleM = m_mLocalTM;

	m_mFinalTM = XMMatrixIdentity();
	m_mFinalTransM = m_mFinalRotM = m_mFinalScaleM = m_mFinalTM;

	m_vPos = VECTOR3(0, 0, 0);
	m_vScale = VECTOR3(1, 1, 1);
	m_vRot = VECTOR3(0, 0, 0);

	m_pVertList = NULL;
	m_pFaceList = NULL;
	m_pCVertList = NULL;
	m_pCFaceList = NULL;

	m_pVerNormalList = NULL;
	m_pVerNrmList_Ex = NULL;
	m_pIndexList_EX = NULL;

	m_pTexUvVertexList = NULL;
	m_pTexUvFaceList = NULL;

	m_AniPosKeyList = NULL;
	// 재질정보

	m_pVB = NULL;
	m_pIB = NULL;

	m_ParentNode = NULL;
	m_ChildNodelist.clear();
	m_bParent = FALSE;

	m_PosKey.clear();
	m_RotKey.clear();

	m_PosKeyCnt = 0;
	m_RotKeyCnt = 0;

	m_AniPosKeyList = NULL;
	m_AniRotKeyList = NULL;

	m_AnimSpeed = 1.0f;
	m_CurrTime = 0.f;
	m_CurrTickTime = 0;
	m_CurrKey = 0;

	m_RotCurrTime = 0;
	m_RotCurrTickTime = 0;
	m_RotCurrKey = 0;

	m_SkinDataList.clear();
	//m_pDev = pDev;


	///
	m_bSkindata = false;
	m_bAniData = false;
	m_bTexData = false;
	m_bSkin = false;
}

AseNode::~AseNode()
{
	// 모델 삭제시 해제
	//Release();
}

int AseNode::_CreateVFBuffers()
{
	int vn = 0;			// 
	int cn = 0;			//

	// 정점색이 없을 경우
	if (m_pCFaceList == NULL)
	{
		for (UINT i = 0; i < m_VertexCnt_Ex; i++)
		{
			m_pVertList[i].dwColor = COLOR(1, 1, 1, 1);

			return 0;
		}
	}

	// 정점색이 있을 경우
	for (UINT i = 0; i < m_FaceCnt; i++)
	{
		vn = m_pFaceList[i].A;						// 페이스별 정점 번호
		cn = m_pCFaceList[i].A;						// 페이스별 정점 색 번호
		m_pVertList[vn].dwColor = m_pCVertList[cn];	// 정점에 정점색 설정

		vn = m_pFaceList[i].B;
		cn = m_pCFaceList[i].B;
		m_pVertList[vn].dwColor = m_pCVertList[cn];


		vn = m_pFaceList[i].C;
		cn = m_pCFaceList[i].C;
		m_pVertList[vn].dwColor = m_pCVertList[cn];
	}

	return OK_SIGN;
}

HRESULT AseNode::_CreateHWBuffers()
{
	HRESULT res = S_OK;

	UINT size = sizeof(VERTEX_ASE_EXTEND) * m_VertexCnt_Ex;
	res = D3DDevice::GetInstance()->CreateVB(m_pVerNrmList_Ex, size, &m_pVB);

	size = sizeof(FACE_ASE) * m_FaceCnt;
	res = D3DDevice::GetInstance()->CreateIB(m_pIndexList_EX, size, &m_pIB);
	return res;
}

int AseNode::Generate()
{
	// 버텍스별 색상 설정...일단 주석처리
	//_CreateVFBuffers();

	HRESULT res = _CreateHWBuffers();
	if (FAILED(res))
	{
		return FAIL_SIGN;
	}

	// 인덱스 버퍼로 그려질 총 정점 개수
	m_IndexCnt = m_FaceCnt * 3;
	return OK_SIGN;
}


DirectX::XMMATRIX AseNode::GetFinalTM()
{
	if (m_ParentNode == NULL)
	{
		return m_mFinalTM;
	}

	return m_mFinalTM * m_ParentNode->GetFinalTM();
}

int AseNode::_ExtendVertexList()
{
	// 새로운 버텍스 버퍼 만들어주기
	int count = m_FaceCnt;
	m_pVerNrmList_Ex = new VERTEX_ASE_EXTEND[count * 3];
	assert(m_pVerNrmList_Ex != NULL);

	(m_pVerNrmList_Ex, sizeof(VERTEX_ASE_EXTEND) * count * 3);

	// 인덱스 버퍼 재구성
	m_pIndexList_EX = new FACE_ASE[count];
	assert(m_pIndexList_EX != NULL);
	ZeroMemory(m_pIndexList_EX, sizeof(FACE_ASE) * count);

	int num = 0;
	// 정점 개수 저장
	m_VertexCnt_Ex = count * 3;

	for (int i = 0; i < count; i++)
	{

		for (int j = 0; j < 3; j++)
		{
			//m_pModel->m_pFaceList[i].A;			// 순서상 1번째
			//m_pModel->m_pFaceList[i].C;			// 순서상 2번째
			//m_pModel->m_pFaceList[i].B;			// 순서상 3번째
			// 순서 유의!!!
			switch (j)
			{
				// Pos값 재구성
			case 0:
				// x
				m_pVerNrmList_Ex[j + num].vPos.x =
					m_pVertList[m_pFaceList[i].A].vPos.x;
				// y
				m_pVerNrmList_Ex[j + num].vPos.y =
					m_pVertList[m_pFaceList[i].A].vPos.y;
				//z
				m_pVerNrmList_Ex[j + num].vPos.z =
					m_pVertList[m_pFaceList[i].A].vPos.z;

				// normal
				m_pVerNrmList_Ex[j + num].vNormal =
					m_pVerNormalList[i].vVtNormal_1;

				if (m_pTexUvVertexList != NULL)
				{
					// uv
					m_pVerNrmList_Ex[j + num].vUv.x =
						m_pTexUvVertexList[m_pTexUvFaceList[i].A].vVertexUV.x;

					m_pVerNrmList_Ex[j + num].vUv.y =
						m_pTexUvVertexList[m_pTexUvFaceList[i].A].vVertexUV.y;
				}
				
				if (m_bSkindata == true)
				{
					// weight
					m_pVerNrmList_Ex[j + num].vbWeight.x =
						m_pVertList[m_pFaceList[i].A].vbWeight.x;

					m_pVerNrmList_Ex[j + num].vbWeight.y =
						m_pVertList[m_pFaceList[i].A].vbWeight.y;

					m_pVerNrmList_Ex[j + num].vbWeight.z =
						m_pVertList[m_pFaceList[i].A].vbWeight.z;

					m_pVerNrmList_Ex[j + num].vbWeight.w =
						m_pVertList[m_pFaceList[i].A].vbWeight.w;

					for (int k = 0; k < 4; k++)
					{
						m_pVerNrmList_Ex[j + num].m_Index[k] = m_pVertList[m_pFaceList[i].A].m_Index[k];
					}

				}

				// 인덱스 버퍼 재구성
				m_pIndexList_EX[i].A = j + num;
				break;
			case 1:
				m_pVerNrmList_Ex[j + num].vPos.x =
					m_pVertList[m_pFaceList[i].C].vPos.x;
				m_pVerNrmList_Ex[j + num].vPos.y =
					m_pVertList[m_pFaceList[i].C].vPos.y;
				m_pVerNrmList_Ex[j + num].vPos.z =
					m_pVertList[m_pFaceList[i].C].vPos.z;
				m_pVerNrmList_Ex[j + num].vNormal =
					m_pVerNormalList[i].vVtNormal_2;

				if (m_pTexUvVertexList != NULL)
				{
					m_pVerNrmList_Ex[j + num].vUv.x =
						m_pTexUvVertexList[m_pTexUvFaceList[i].C].vVertexUV.x;

					m_pVerNrmList_Ex[j + num].vUv.y =
						m_pTexUvVertexList[m_pTexUvFaceList[i].C].vVertexUV.y;
				}

				if (m_bSkindata == true)
				{
					m_pVerNrmList_Ex[j + num].vbWeight.x =
						m_pVertList[m_pFaceList[i].C].vbWeight.x;

					m_pVerNrmList_Ex[j + num].vbWeight.y =
						m_pVertList[m_pFaceList[i].C].vbWeight.y;

					m_pVerNrmList_Ex[j + num].vbWeight.z =
						m_pVertList[m_pFaceList[i].C].vbWeight.z;

					m_pVerNrmList_Ex[j + num].vbWeight.w =
						m_pVertList[m_pFaceList[i].C].vbWeight.w;


					for (int k = 0; k < 4; k++)
					{
						m_pVerNrmList_Ex[j + num].m_Index[k] = m_pVertList[m_pFaceList[i].C].m_Index[k];
					}
				}

				// 인덱스 버퍼 재구성
				m_pIndexList_EX[i].B = j + num;
				break;
			case 2:
				m_pVerNrmList_Ex[j + num].vPos.x =
					m_pVertList[m_pFaceList[i].B].vPos.x;
				m_pVerNrmList_Ex[j + num].vPos.y =
					m_pVertList[m_pFaceList[i].B].vPos.y;
				m_pVerNrmList_Ex[j + num].vPos.z =
					m_pVertList[m_pFaceList[i].B].vPos.z;

				m_pVerNrmList_Ex[j + num].vNormal =
					m_pVerNormalList[i].vVtNormal_3;

				if (m_pTexUvVertexList != NULL)
				{
					m_pVerNrmList_Ex[j + num].vUv.x =
						m_pTexUvVertexList[m_pTexUvFaceList[i].B].vVertexUV.x;

					m_pVerNrmList_Ex[j + num].vUv.y =
						m_pTexUvVertexList[m_pTexUvFaceList[i].B].vVertexUV.y;
				}
				if (m_bSkindata == true)
				{
					// weight
					m_pVerNrmList_Ex[j + num].vbWeight.x =
						m_pVertList[m_pFaceList[i].B].vbWeight.x;

					m_pVerNrmList_Ex[j + num].vbWeight.y =
						m_pVertList[m_pFaceList[i].B].vbWeight.y;

					m_pVerNrmList_Ex[j + num].vbWeight.z =
						m_pVertList[m_pFaceList[i].B].vbWeight.z;

					m_pVerNrmList_Ex[j + num].vbWeight.w =
						m_pVertList[m_pFaceList[i].B].vbWeight.w;

					for (int k = 0; k < 4; k++)
					{
						m_pVerNrmList_Ex[j + num].m_Index[k] = m_pVertList[m_pFaceList[i].B].m_Index[k];
					}
				}
				// 인덱스 버퍼 재구성
				m_pIndexList_EX[i].C = j + num;
				break;
			}
		}
		// 3개니깐 플러스 3..... 상수를 없앨 방법을 생각해보자!!!
		num += 3;
		//continue;
	}
	return OK_SIGN;
}

void AseNode::PushChildNode(AseNode* pnode)
{
	m_ChildNodelist.push_back(pnode);
}

void AseNode::Release()
{
	//임시 버퍼 삭제.
	SAFE_DELETE(m_pVertList);
	SAFE_DELETE(m_pFaceList);
	SAFE_DELETE(m_pCVertList);
	SAFE_DELETE(m_pCFaceList);
	SAFE_DELETE(m_pVerNormalList);
	SAFE_DELETE(m_pVerNrmList_Ex);
	SAFE_DELETE(m_pIndexList_EX);
	SAFE_DELETE(m_pTexUvVertexList);

	//렌더링 버퍼 제거.
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
}

DirectX::XMFLOAT4X4 AseNode::Xmfloat4x4Identity()
{
	XMFLOAT4X4 temp;
	temp._11 = 1;
	temp._12 = 0;
	temp._13 = 0;
	temp._14 = 0;
	temp._21 = 0;
	temp._22 = 1;
	temp._23 = 0;
	temp._24 = 0;
	temp._31 = 0;
	temp._32 = 0;
	temp._33 = 1;
	temp._34 = 0;
	temp._41 = 0;
	temp._42 = 0;
	temp._43 = 0;
	temp._44 = 1;

	return temp;
}

