//
//
//
//
// [2020/07/28 RUNSUMIN]

#include "AseModel.h"
AseModel::AseModel()
{
	_tcscpy(m_FileName, L"NA");
	_tcscpy(m_NodeName, L"NA");

	// 노드들을 담을 리스트 초기화
	m_NodeList.clear();
	m_BoneList.clear();
	m_CollList.clear();

	m_RootNode = NULL;

	m_RootNodeList.clear();

	// 모델 운용 변수들 초기화

	// 각각 노드들의 버퍼를 다르게 운용한다
	// 초기화 하면 프로그램 뻗는다... 이유 모름
	//m_LasrFrame = 0;
	//m_FrameSpeed = 0;
	m_CurrTime = 0;
	m_SaveTime = 0.f;
}

AseModel::~AseModel()
{
	// 추가된 사이즈 만큼 제거
	AseModelRelease();
}

AseNode* AseModel::FindParentNodeName(const TCHAR* name)
{
	for (int i = 0; i < m_BoneList.size(); i++)
	{
		const TCHAR* nodeName = m_BoneList[i]->GetNodeName();

		if (EQUAL(nodeName, name))
			return m_BoneList[i];
	}

	for (int i = 0; i < m_NodeList.size(); i++)
	{
		const TCHAR* nodeName = m_NodeList[i]->GetNodeName();

		if (EQUAL(nodeName, name))
			return m_NodeList[i];
	}

	return nullptr;
}

int AseModel::FindBoneindexbyName(const TCHAR* name)
{

	for (int i = 0; i < m_BoneList.size(); i++)
	{
		const TCHAR* nodeName = m_BoneList[i]->GetNodeName();

		if (_tcsicmp(nodeName, name) == 0)
		{
			return i;
		}
	}

	return 0;
}

void AseModel::MakeTM(AseNode* pnode)
{
	XMMATRIX _mTM = XMLoadFloat4x4(&pnode->m_mTM);

	XMVECTOR scale;
	XMVECTOR rot;
	XMVECTOR transe;

	if (pnode->m_ParentNode == NULL)
	{
		XMMatrixDecompose(&scale, &rot, &transe, _mTM);
	}
	else
	{
		XMMATRIX inv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&pnode->m_ParentNode->m_mTM));
		_mTM = _mTM * inv;
		XMMatrixDecompose(&scale, &rot, &transe, _mTM);
	}

	pnode->m_mLocalScaleM = XMMatrixScalingFromVector(scale);
	pnode->m_mLocalRotM = XMMatrixRotationQuaternion(rot);
	pnode->m_mLocalTransM = XMMatrixTranslationFromVector(transe);

	pnode->m_mLocalTM = pnode->m_mLocalScaleM * pnode->m_mLocalRotM * pnode->m_mLocalTransM;

	int cnt = pnode->m_ChildNodelist.size();

	for (int i = 0; i < cnt; i++)
	{
		MakeTM(pnode->m_ChildNodelist[i]);
	}

}

void AseModel::MakeTM_Fors(std::vector<AseNode*> pnodeList)
{
	int cnt = pnodeList.size();
	for (int i = 0; i < cnt; i++)
	{
		XMMATRIX _mTM = XMLoadFloat4x4(&pnodeList[i]->m_mTM);

		XMVECTOR scale;
		XMVECTOR rot;
		XMVECTOR transe;

		if (pnodeList[i]->m_ParentNode == NULL)
		{
			XMMatrixDecompose(&scale, &rot, &transe, _mTM);
		}
		else
		{
			XMMATRIX inv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&pnodeList[i]->m_ParentNode->m_mTM));
			_mTM = _mTM * inv;
			XMMatrixDecompose(&scale, &rot, &transe, _mTM);
		}

		pnodeList[i]->m_mLocalScaleM = XMMatrixScalingFromVector(scale);
		pnodeList[i]->m_mLocalRotM = XMMatrixRotationQuaternion(rot);
		pnodeList[i]->m_mLocalTransM = XMMatrixTranslationFromVector(transe);

		pnodeList[i]->m_mLocalTM = pnodeList[i]->m_mLocalScaleM * pnodeList[i]->m_mLocalRotM * pnodeList[i]->m_mLocalTransM;

		int cnt = pnodeList[i]->m_ChildNodelist.size();

		for (int j = 0; j < cnt; j++)
		{
			MakeTM_Fors(pnodeList[i]->m_ChildNodelist);
		}
	}


}

void AseModel::MakeFinalTm()
{
	// 최종 행렬 만들어 주는 부분..... 유저 TM도 넘겨 주어야 한다
	MakeTM_Fors(m_RootNodeList);
	// 충돌 범위 월드 좌표로 만들어 주기
	MakeTM_Fors(m_CollList);


	int cnt = m_NodeList.size();
	for (int i = 0; i < cnt; i++)
	{
		m_NodeList[i]->m_mAniTransM = m_NodeList[i]->m_mLocalTransM;
		m_NodeList[i]->m_mAniRotM = m_NodeList[i]->m_mLocalRotM;
		m_NodeList[i]->m_mAniScaleM = m_NodeList[i]->m_mLocalScaleM;

		// 최종 행렬 계산...
		m_NodeList[i]->m_mAniTM = m_NodeList[i]->m_mAniScaleM * m_NodeList[i]->m_mAniRotM * m_NodeList[i]->m_mAniTransM;

		m_NodeList[i]->m_mFinalTM = m_NodeList[i]->m_mAniTM;
	}

}

int AseModel::initializeModel()
{
	MakeTM_Fors(m_RootNodeList);
	// 충돌 범위 월드 좌표로 만들어 주기
	MakeTM_Fors(m_CollList);
	return TRUE;
}

void AseModel::AseModelRelease()
{
	// 추가된 사이즈 만큼 제거(모델)
	int cnt = m_NodeList.size();
	for (int i = 0; i < cnt; i++)
	{
		m_NodeList[i]->Release();
	}

	// 추가된 사이즈 만큼 제거(본)
	cnt = m_BoneList.size();
	for (int i = 0; i < cnt; i++)
	{
		m_BoneList[i]->Release();
	}

	// 추가된 사이즈 만큼 제거(충돌 영역)
	cnt = m_CollList.size();
	for (int i = 0; i < cnt; i++)
	{
		m_CollList[i]->Release();
	}
}

int AseModel::ShowModelInfo()
{
	return OK_SIGN;
}

int AseModel::Generate()
{
	int cnt = m_NodeList.size();
	for (int i = 0; i < cnt; i++)
	{
		m_NodeList[i]->Generate();
	}

	cnt = m_BoneList.size();
	for (int i = 0; i < cnt; i++)
	{
		m_BoneList[i]->Generate();
	}

	cnt = m_CollList.size();
	for (int i = 0; i < cnt; i++)
	{
		m_CollList[i]->Generate();
	}
	return OK_SIGN;
}

int ModelCreateFromASE(ID3D11Device* pDev, TCHAR* filename, AseModel** ppModel, ID3D11DeviceContext* pDevContext)
{
	// 파서 생성
	AseParser* Paser = new AseParser;

	if (Paser == NULL)
	{
		// 파서 생성 실패 오류 출력
		return FAIL_SIGN;
	}

	// AseModel 생성

	AseModel* Model = new AseModel();
	assert(Model != NULL);

	// 데이터 로드
	if (Paser->Load(pDev, filename, Model, pDevContext) == FAIL_SIGN)
	{
		// 데이터 로드 실패시 오류 출력
		SAFE_DELETE(Paser);
		SAFE_DELETE(Model);
		*ppModel = NULL;
		return FAIL_SIGN;
	}

	if (Model->Generate() == FAIL_SIGN)
	{
		SAFE_DELETE(Paser);
		SAFE_DELETE(Model);
		*ppModel = NULL;
		return FAIL_SIGN;
	}

	*ppModel = Model;			//모델 리턴..
	SAFE_DELETE(Paser);			//파서 제거..

	return OK_SIGN;
}

void ModelRelease(AseModel*& ppModel)
{
	SAFE_DELETE(ppModel);
	ppModel = NULL;
}
