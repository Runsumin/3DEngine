#include "FBX_StaticMesh.h"

FBX_StaticMesh::FBX_StaticMesh()
	:m_UserTm(Matrix::Identity), m_Trans(Matrix::Identity), m_Rot(Matrix::Identity), m_Scale(Matrix::Identity),
	m_vPos(Vector3::Zero), m_vScl(Vector3::Zero), m_vRot(Vector3::Zero), m_TexCount(0), m_bNorOn(false), m_LightVP(Matrix::Identity),
	m_TexOn(false), m_AniOn(false), m_SKinOn(false), m_AnimationSpeed(1.0f), m_NowAniCount(0), m_Aniloop(false), m_NowFrameCount(0),
	m_CurrTime(0.f), m_AniOnOff(false)
{
	// 장치 정보 받아오기
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();
}

FBX_StaticMesh::~FBX_StaticMesh()
{
	ReleaseFbxModel();
}

void FBX_StaticMesh::InitFbxModel()
{
	//일단은 순서상 모델은 먼저 로드하고 다음 과정이다
	//////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < m_pFbxDataList.size(); i++)
	{
		m_pFbxDataList[i]->CreateBiNorNTangent();
	}

	CreateHWBuffer();
	InitShader();

	if (m_Texfilename.size() > 0)
	{
		SetTextureDateInit();
	}
	/// 상수버퍼 정보 초기화
	ZeroMemory(&m_CbDef, sizeof(CBFBXDEF));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBFBXDEF), &m_CbDef, &m_pCbDefBuf, m_pDevCopy);

	/// 상수버퍼 정보 초기화
	ZeroMemory(&m_CbMat, sizeof(CBFBXMAT));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBFBXMAT), &m_CbMat, &m_pCbMatBuf, m_pDevCopy);

	/// 상수버퍼 정보 초기화
	ZeroMemory(&m_CbLight, sizeof(CBLIGHT));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBLIGHT), &m_CbLight, &m_pCbLightBuf, m_pDevCopy);


	if (m_pAniDataList[0]->AnimationList.size() > 0)
	{
		m_AniOn = true;
	}

}

void FBX_StaticMesh::CreateHWBuffer()
{
	int cnt = m_pFbxDataList.size();
	for (int i = 0; i < cnt; i++)
	{
		HRESULT res = S_OK;

		FbxModelData* m_pdata = new FbxModelData();

		UINT size = sizeof(Vertex) * m_pFbxDataList[i]->m_pVertexList.size();

		res = D3DDevice::GetInstance()->CreateVB(m_pFbxDataList[i]->m_pVertexList.data(), size, &m_pdata->m_pVB);

		if (FAILED(res))
		{
			cout << "버텍스 버퍼 구성 실패..." << endl;
		}

		size = sizeof(Index) * m_pFbxDataList[i]->m_pIndexList.size();
		res = D3DDevice::GetInstance()->CreateIB(m_pFbxDataList[i]->m_pIndexList.data(), size, &m_pdata->m_pIB);

		if (FAILED(res))
		{
			cout << "인덱스 버퍼 구성 실패..." << endl;
		}
		m_pdata->m_pFbxData = m_pFbxDataList[i];

		m_pFinalDatalist.push_back(m_pdata);
	}

}

void FBX_StaticMesh::InitShader()
{
	/// 모델에 사용할 셰이더 초기화
	m_pShader = new Shader();

	m_pShader->ShaderLoad(L"../shader/Normal_Mapping.fx", m_pDevCopy);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//Sementic            format							 offset classification             
		{ "POSITION",	   0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",   0, DXGI_FORMAT_R32G32B32A32_FLOAT,	 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",  0, DXGI_FORMAT_R32G32B32A32_UINT,	 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		   0, DXGI_FORMAT_R32G32B32_FLOAT,       0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL",	   0, DXGI_FORMAT_R32G32B32_FLOAT,       0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	   0, DXGI_FORMAT_R32G32B32_FLOAT,       0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",      0, DXGI_FORMAT_R32G32_FLOAT,		     0, 84, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	UINT numElements = ARRAYSIZE(layout);

	m_pShader->CreateVBLayOut(layout, numElements, m_pDevCopy);

}

void FBX_StaticMesh::SetTextureDateInit()
{
	int cnt = m_Texfilename.size();
	for (int i = 0; i < cnt; i++)
	{
		ID3D11ShaderResourceView* pTex;
		string finalpath = m_TexPath + m_Texfilename[i];
		HRESULT hr;
		hr = D3DDevice::GetInstance()->LoadTexture(finalpath, &pTex);
		if (FAILED(hr))
		{

		}
		else
		{
			m_FbxModelTexture.push_back(pTex);
		}
	}

	cnt = m_Norfilename.size();
	for (int i = 0; i < cnt; i++)
	{
		ID3D11ShaderResourceView* pTex;
		string finalpath = m_TexPath + m_Norfilename[i];
		HRESULT hr;
		hr = D3DDevice::GetInstance()->LoadTexture(finalpath, &pTex);
		if (FAILED(hr))
		{

		}
		else
		{
			m_FbxModelNormal.push_back(pTex);
		}
	}

	cnt = m_MaskMapfilename.size();
	for (int i = 0; i < cnt; i++)
	{
		ID3D11ShaderResourceView* pTex;
		string finalpath = m_TexPath + m_MaskMapfilename[i];
		HRESULT hr;
		hr = D3DDevice::GetInstance()->LoadTexture(finalpath, &pTex);
		if (FAILED(hr))
		{

		}
		else
		{
			m_FbxModelMaskMap.push_back(pTex);
		}
	}
}

void FBX_StaticMesh::ModelAniUpdate(float time, int anicount, int keycount)
{
	Matrix identityTM;
	vector<Matrix> bonematdata;

	for (int j = 0; j < m_pBoneList_Hierarchy.size(); j++)
	{
		ModelAnimationUpdate_Recur_Temp(anicount, m_pBoneList_Hierarchy[j], m_CurrTime, identityTM, bonematdata, keycount);
	}

	for (int i = 0; i < m_pAllBoneList.size(); i++)
	{
		m_CbDef.m_BoneTm[i] = m_pAllBoneList[i]->GlobalBindingTm * bonematdata[i];
	}

}

void FBX_StaticMesh::ModelAnimationUpdate_Recur_Temp(int anicount, Bone* pbone, float currtime, Matrix parenttm, vector<Matrix>& bonematdata, int nowframecount)
{
	/// 애니메이션이 하나라고 가정...

	int boneindex = pbone->BoneIndex;
	Matrix GlobalbindingTM = pbone->GlobalBindingTm;
	const OneFrameList* oneframe = m_pAniDataList[anicount]->AnimationList[boneindex];

	Matrix anitm;

	if (oneframe == NULL)
	{
		anitm = GlobalbindingTM;
	}
	else
	{
		KeyFrameData firstkey = oneframe->FrmList.front();
		KeyFrameData Endkey = oneframe->FrmList.back();

		if (currtime <= firstkey.Time)
		{
			XMVECTOR vQuatRot = XMLoadFloat4(&firstkey.Rot_Quat);
			XMVECTOR vTrans = XMLoadFloat3(&firstkey.Trans);
			XMVECTOR vScale = XMLoadFloat3(&firstkey.Scale);

			XMMATRIX mRot = XMMatrixRotationQuaternion(vQuatRot);
			XMMATRIX mTras = XMMatrixTranslationFromVector(vTrans);
			XMMATRIX mScale = XMMatrixScalingFromVector(vScale);

			XMMATRIX mTransform = mScale * mRot * mTras;

			XMStoreFloat4x4(&anitm, mTransform);
		}
		else if (currtime >= Endkey.Time)
		{
			XMVECTOR vQuatRot = XMLoadFloat4(&Endkey.Rot_Quat);
			XMVECTOR vTrans = XMLoadFloat3(&Endkey.Trans);
			XMVECTOR vScale = XMLoadFloat3(&Endkey.Scale);

			XMMATRIX mRot = XMMatrixRotationQuaternion(vQuatRot);
			XMMATRIX mTras = XMMatrixTranslationFromVector(vTrans);
			XMMATRIX mScale = XMMatrixScalingFromVector(vScale);

			XMMATRIX mTransform = mScale * mRot * mTras;

			XMStoreFloat4x4(&anitm, mTransform);
		}
		else
		{
			KeyFrameData currkey;
			KeyFrameData nextkey;

			//for (int i = 0; i + 1 < oneframe->FrmList.size(); i++)
			//{
			//	if (currtime > oneframe->FrmList[i].Time && currtime < oneframe->FrmList[i + 1].Time)
			//	{
			//		break;
			//	}
			//}
			currkey = oneframe->FrmList[nowframecount];
			nextkey = oneframe->FrmList[nowframecount + 1];

			float totalTime = nextkey.Time - currkey.Time;
			float lerpValue = (currtime - currkey.Time) / totalTime;

			XMVECTOR vQuatRot0 = XMLoadFloat4(&currkey.Rot_Quat);
			XMVECTOR vTrans0 = XMLoadFloat3(&currkey.Trans);
			XMVECTOR vScale0 = XMLoadFloat3(&currkey.Scale);

			XMVECTOR vQuatRot1 = XMLoadFloat4(&nextkey.Rot_Quat);
			XMVECTOR vTrans1 = XMLoadFloat3(&nextkey.Trans);
			XMVECTOR vScale1 = XMLoadFloat3(&nextkey.Scale);

			XMVECTOR vQuatRot = XMQuaternionSlerp(vQuatRot0, vQuatRot1, lerpValue);
			XMVECTOR vTrans = XMVectorLerp(vTrans0, vTrans1, lerpValue);
			XMVECTOR vScale = XMVectorLerp(vScale0, vScale1, lerpValue);

			XMMATRIX mRot = XMMatrixRotationQuaternion(vQuatRot);
			XMMATRIX mTras = XMMatrixTranslationFromVector(vTrans);
			XMMATRIX mScale = XMMatrixScalingFromVector(vScale);

			XMMATRIX mTransform = mScale * mRot * mTras;

			XMStoreFloat4x4(&anitm, mTransform);


		}
	}

	bonematdata.push_back(anitm);

	for (int i = 0; i < pbone->pChildBoneList.size(); i++)
	{
		ModelAnimationUpdate_Recur_Temp(anicount, pbone->pChildBoneList[i], currtime, anitm, bonematdata, nowframecount);
	}

}

void FBX_StaticMesh::SetPrimitiveData(int nodecount)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	m_pDevConCopy->IASetVertexBuffers(0, 1, &m_pFinalDatalist[nodecount]->m_pVB, &stride, &offset);
	m_pDevConCopy->IASetIndexBuffer(m_pFinalDatalist[nodecount]->m_pIB, DXGI_FORMAT_R32_UINT, offset);
	//입력 레이아웃 설정. Set the input layout
	m_pDevConCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	// 기하 위상 구조 설정 Set primitive topology
	m_pDevConCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void FBX_StaticMesh::SetTextrueData(int nodecount)
{
	// 텍스쳐가 존재 할 때
	if (m_FbxModelTexture.size() > 0)
	{
		if (m_pFinalDatalist[nodecount]->m_pFbxData->m_TextureCount != 100)
		{
			m_pDevConCopy->PSSetShaderResources(0, 1, &m_FbxModelTexture[m_pFinalDatalist[nodecount]->m_pFbxData->m_TextureCount]);
		}
	}

	if (m_pFinalDatalist[nodecount]->m_pFbxData->m_NormalMapOn == true)
	{
		if (m_FbxModelNormal.size() > 0)
		{
			if (m_pFinalDatalist[nodecount]->m_pFbxData->m_NormalCount != 100)
			{
				m_pDevConCopy->PSSetShaderResources(1, 1, &m_FbxModelNormal[m_pFinalDatalist[nodecount]->m_pFbxData->m_NormalCount]);
			}
		}

	}

	if (m_pFinalDatalist[nodecount]->m_pFbxData->m_MaskMapOn == true)
	{
		if (m_FbxModelMaskMap.size() > 0)
		{
			if (m_pFinalDatalist[nodecount]->m_pFbxData->m_MaskMapCount != 100)
			{
				m_pDevConCopy->PSSetShaderResources(2, 1, &m_FbxModelMaskMap[m_pFinalDatalist[nodecount]->m_pFbxData->m_MaskMapCount]);
			}
		}
	}

	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_WRAP]);
	m_pDevConCopy->PSSetSamplers(1, 1, &D3DDevice::GetInstance()->m_pSampler[SS_BOARDER]);

}

void FBX_StaticMesh::SetMaterialData(int nodecount)
{
	if (m_pFinalDatalist[nodecount]->m_pFbxData->m_pMaterial != NULL)
	{
		m_CbMat.Diffuse = m_pFinalDatalist[nodecount]->m_pFbxData->m_pMaterial->Diffuse;
		m_CbMat.Ambient = m_pFinalDatalist[nodecount]->m_pFbxData->m_pMaterial->Ambient;
		m_CbMat.Specular = m_pFinalDatalist[nodecount]->m_pFbxData->m_pMaterial->Specular;
		m_CbMat.TexOn = m_pFinalDatalist[nodecount]->m_pFbxData->m_TextureOn;
		m_CbMat.NormalOn = m_pFinalDatalist[nodecount]->m_pFbxData->m_NormalMapOn;
		m_CbMat.SkinOn = m_AniOn;
		m_CbMat.MaskOn = m_pFinalDatalist[nodecount]->m_pFbxData->m_MaskMapOn;
	}
}

void FBX_StaticMesh::SetConstantBuffer(int nodecount)
{
	// 상수 버퍼 데이터 전달
	m_CbDef.m_AxisChange = m_AxisChange;
	m_CbDef.mTm = m_pFinalDatalist[nodecount]->m_pFbxData->m_Worldm * m_UserTm;
	m_CbDef.mWorld = m_pFinalDatalist[nodecount]->m_pFbxData->m_Worldm;
	m_CbDef.mWV = m_CbDef.mTm * m_CbDef.mView;
	m_CbDef.mWVP = m_CbDef.mTm * m_CbDef.mView * m_CbDef.mProj;

	// 상수 버퍼
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbDefBuf, &m_CbDef, sizeof(CBFBXDEF));

	// 조명 정보
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbLightBuf, &m_CbLight, sizeof(CBLIGHT));

	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbMatBuf, &m_CbMat, sizeof(CBFBXMAT));

	//기본 셰이더 설정.
	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	//기본 셰이더 상수버퍼 설정.
	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pCbDefBuf);
	m_pDevConCopy->VSSetConstantBuffers(1, 1, &m_pCbMatBuf);
	m_pDevConCopy->VSSetConstantBuffers(2, 1, &m_pCbLightBuf);

	// 픽셀 셰이더 상수 버퍼 설정.
	m_pDevConCopy->PSSetConstantBuffers(0, 1, &m_pCbDefBuf);
	m_pDevConCopy->PSSetConstantBuffers(1, 1, &m_pCbMatBuf);
	m_pDevConCopy->PSSetConstantBuffers(2, 1, &m_pCbLightBuf);

}

void FBX_StaticMesh::Draw(int nodecount)
{
	// 드로우 방식 변경... instancing
	if (m_pFinalDatalist[nodecount]->m_pFbxData->m_NodeName == "wing")
	{
		D3DDevice::GetInstance()->ChangeRenderState(GRAPHICENGINE::RenderState::SOLID);
		m_pDevConCopy->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_ON], NULL, 0xFFFFFFFF);
		m_pDevConCopy->DrawIndexed(m_pFinalDatalist[nodecount]->m_pFbxData->m_pVertexList.size(), 0, 0);
		m_pDevConCopy->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_OFF], NULL, 0xFFFFFFFF);
		D3DDevice::GetInstance()->ChangeRenderState(GRAPHICENGINE::RenderState::CULLBACK);
	}
	else if (m_pFinalDatalist[nodecount]->m_pFbxData->m_NodeName == "fur")
	{
		m_pDevConCopy->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_ON], NULL, 0xFFFFFFFF);
		m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_WRITE_OFF_STENCIL_ON], 0);
		m_pDevConCopy->DrawIndexed(m_pFinalDatalist[nodecount]->m_pFbxData->m_pVertexList.size(), 0, 0);
		m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON_STENCIL_ON], 0);
		m_pDevConCopy->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_OFF], NULL, 0xFFFFFFFF);
	}
	else
	{
		m_pDevConCopy->DrawIndexed(m_pFinalDatalist[nodecount]->m_pFbxData->m_pVertexList.size(), 0, 0);
	}
}

int FBX_StaticMesh::CalAniFrame(float Time)
{
	float totaltime = m_pAniDataList[m_NowAniCount]->TotalTime;
	int  totalframe = m_pAniDataList[m_NowAniCount]->TotalFrames;
	float dtime = Time;
	float currtime = fmodf(dtime, totaltime);

	m_CurrTime = currtime;
	int framecount = currtime / (totaltime / totalframe);

	return framecount;
}

void FBX_StaticMesh::FbxModelUpdate(float time)
{
	// 카메라 행렬 정보 전달
	m_CbDef.mView = D3DDevice::GetInstance()->m_View;
	m_CbDef.mProj = D3DDevice::GetInstance()->m_Proj;

	m_CbLight = D3DDevice::GetInstance()->GiveDirectionLightData();

	if (m_AniOn == true && m_AniOnOff == true)
	{
		m_NowFrameCount = CalAniFrame(m_AniTime);

		if (m_NowFrameCount <= m_pAniDataList[m_NowAniCount]->TotalFrames -2  && m_Aniloop == false)
		{
			m_AniTime += time * m_AnimationSpeed;
		}
		else if (m_Aniloop == true)
		{
			m_AniTime += time * m_AnimationSpeed;
		}

	}

	ModelAniUpdate(m_AniTime, m_NowAniCount, m_NowFrameCount);

}
void FBX_StaticMesh::FbxModelDraw(float time)
{
	// 알파 소팅에 대한 랜더 패스가 필요하다.

	int cnt = m_pFinalDatalist.size();

	for (int i = 0; i < cnt; i++)
	{
		if (m_pFinalDatalist[i]->m_pFbxData->m_MaskMapOn == false)
		{
			SetPrimitiveData(i);
			SetTextrueData(i);
			SetMaterialData(i);
			SetConstantBuffer(i);
			Draw(i);
		}
	}

	for (int i = 0; i < cnt; i++)
	{
		if (m_pFinalDatalist[i]->m_pFbxData->m_MaskMapOn == true)
		{
			SetPrimitiveData(i);
			SetTextrueData(i);
			SetMaterialData(i);
			SetConstantBuffer(i);
			Draw(i);
		}
	}
}

void FBX_StaticMesh::FbxModelInfo()
{

}

void FBX_StaticMesh::ReleaseFbxModel()
{
	deletevector(m_pFbxDataList);
	m_pFbxDataList.clear();
	deletevector(m_pAllBoneList);
	m_pAllBoneList.clear();
	m_pBoneList_Hierarchy.clear();
	deletevector(m_pAniDataList);
	m_pAniDataList.clear();
	///
	for (int i = 0; i < m_FbxModelTexture.size(); i++)
	{
		SAFE_RELEASE(m_FbxModelTexture[i]);
	}
	m_FbxModelTexture.clear();
	for (int i = 0; i < m_FbxModelNormal.size(); i++)
	{
		SAFE_RELEASE(m_FbxModelNormal[i]);
	}
	m_FbxModelNormal.clear();
	for (int i = 0; i < m_FbxModelMaskMap.size(); i++)
	{
		SAFE_RELEASE(m_FbxModelMaskMap[i]);
	}
	m_FbxModelMaskMap.clear();

	SAFE_RELEASE(m_pCbDefBuf);
	SAFE_RELEASE(m_pCbMatBuf);
	SAFE_RELEASE(m_pCbLightBuf);

	for (int i = 0; i < m_pFinalDatalist.size(); i++)
	{
		SAFE_RELEASE(m_pFinalDatalist[i]->m_pIB);
		SAFE_RELEASE(m_pFinalDatalist[i]->m_pVB);
	}

	SAFE_RELEASE(m_pDevCopy);
	SAFE_RELEASE(m_pDevConCopy);

	m_pShader->ShaderRelease();
}

void FBX_StaticMesh::SetUserMatrix(Matrix usertm)
{
	m_UserTm = usertm;
}

void FBX_StaticMesh::SetAnimationSpeed(float speed)
{
	m_AnimationSpeed = speed;
}

void FBX_StaticMesh::ChangeAnimation(int count)
{
	m_NowAniCount = count;
	// 현재 애니메이션 프레임을 0으로 만들어 준다
	m_AniTime = 0;
}

void FBX_StaticMesh::SetAnimationLoop(bool loop)
{
	m_Aniloop = loop;
}

float FBX_StaticMesh::GetAniTotalTime()
{
	float totaltime = m_pAniDataList[m_NowAniCount]->TotalTime;

	return totaltime;
}

bool FBX_StaticMesh::GetAniEndTimeing()
{
	if (m_NowFrameCount >= m_pAniDataList[m_NowAniCount]->TotalFrames - 2)
	{
		return true;
	}
	return false;
}

int FBX_StaticMesh::GetAniTotalKeyCount()
{
	return m_pAniDataList[m_NowAniCount]->TotalFrames;
}

int FBX_StaticMesh::GetAniNowKeyCount()
{
	return m_NowFrameCount;
}

void FBX_StaticMesh::SetNowKeyCount(int key)
{
	m_NowFrameCount = key;
}

void FBX_StaticMesh::SetAniOn()
{
	m_AniOnOff = true;
}

void FBX_StaticMesh::SetAniOff()
{
	m_AniOnOff = false;
}

std::string FBX_StaticMesh::GetModelName()
{
	return m_ModelName;
}

