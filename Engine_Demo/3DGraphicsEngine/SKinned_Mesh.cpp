#include "SKinned_Mesh.h"
#include "Material.h"
SKinned_Mesh::SKinned_Mesh()
	:m_UserTm(Matrix::Identity), m_Trans(Matrix::Identity), m_Rot(Matrix::Identity), m_Scale(Matrix::Identity),
	m_vPos(Vector3::Zero), m_vScl(Vector3::Zero), m_vRot(Vector3::Zero), m_bNorOn(false), m_LightVP(Matrix::Identity),
	m_AniOn(false), m_AnimationSpeed(1.0f), m_NowAniCount(0), m_Aniloop(false), m_NowFrameCount(0),
	m_CurrTime(0.f), m_AniOnOff(false), m_NowNodeCount(0),
	m_TexOn(false), m_NormalOn(false), m_MaskOn(false), m_BumpMappingOn(false), m_PBROn(false)
{
	// 장치 정보 받아오기
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();
	m_pShader = ShaderMgr::GetInstance()->m_pSkinned;
	m_pPsDepth = ShaderMgr::GetInstance()->m_pPsDepth;
	m_Draw = true;
	m_ModelRenderState = GRAPHICENGINE::RenderState::CULLBACK;
}

SKinned_Mesh::~SKinned_Mesh()
{

}

bool SKinned_Mesh::SetModelData(Fbx_MeshData* pdata)
{
	m_pModelData = pdata;
	if (m_pModelData == nullptr)
		return false;
	else
		return true;
}

void SKinned_Mesh::InitFbxModel()
{
	//일단은 순서상 모델은 먼저 로드하고 다음 과정이다
//////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < m_pModelData->m_pFbxDataList.size(); i++)
	{
		m_pModelData->m_pFbxDataList[i]->CreateBiNorNTangent();
	}

	CreateHWBuffer();

	if (m_pModelData->m_AutoTextureSet == true)
	{
		GetShaderResource();
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

	/// 상수버퍼 정보 초기화
	ZeroMemory(&m_CBFog, sizeof(CBFOG));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBFOG), &m_CBFog, &m_pCbFogBuf, m_pDevCopy);

	/// 상수버퍼 정보 초기화
	ZeroMemory(&m_ChannelView, sizeof(CHANNELVIEW));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CHANNELVIEW), &m_ChannelView, &m_pCbChannelViewBuf, m_pDevCopy);

	int anicount = m_pModelData->m_pAniDataList[0]->AnimationList.size();
	if (anicount > 0)
	{
		m_AniOn = true;
	}

	int size = m_pModelData->m_pAllBoneList.size();
	m_AniBoneMatData.resize(size);

	/// 모델 초기 본 매트릭스 업데이트
	ModelAniUpdate(0, 0, 0);
	
}

void SKinned_Mesh::InitModelTexture()
{
	GetShaderResource();
}

void SKinned_Mesh::CreateHWBuffer()
{
	int cnt = m_pModelData->m_pFbxDataList.size();
	for (int i = 0; i < cnt; i++)
	{
		HRESULT res = S_OK;

		FbxModelData* m_pdata = new FbxModelData();

		UINT size = sizeof(Vertex) * m_pModelData->m_pFbxDataList[i]->m_pVertexList.size();

		res = D3DDevice::GetInstance()->CreateVB(m_pModelData->m_pFbxDataList[i]->m_pVertexList.data(), size, &m_pdata->m_pVB);

		if (FAILED(res))
		{
			cout << "버텍스 버퍼 구성 실패..." << endl;
		}

		size = sizeof(Index) * m_pModelData->m_pFbxDataList[i]->m_pIndexList.size();
		res = D3DDevice::GetInstance()->CreateIB(m_pModelData->m_pFbxDataList[i]->m_pIndexList.data(), size, &m_pdata->m_pIB);

		if (FAILED(res))
		{
			cout << "인덱스 버퍼 구성 실패..." << endl;
		}
		m_pdata->m_pFbxData = m_pModelData->m_pFbxDataList[i];

		// 기본적으로 렌더 큐에 알파가 없는 모델로 들어간다.
		if (m_pModelData->m_pFbxDataList[i]->m_MaskMapOn == true)
		{
			m_pdata->m_QueueSrot = GRAPHICENGINE::RenderQueueSort::SORT_ALPHATEST;
		}
		else
		{
			m_pdata->m_QueueSrot = GRAPHICENGINE::RenderQueueSort::SORT_TRANSPARENT;
		}
		m_pFinalDatalist.push_back(m_pdata);
	}

	float radious1 = m_pModelData->m_BiggstY - m_pModelData->m_leastY;
	float radious2 = m_pModelData->m_BiggstX - m_pModelData->m_leastX;
	m_Radious = radious1;
	GRAPHICENGINE::CreateGeometry(&m_CollBox, GRAPHICENGINE::Geomerty_Sort::SPHERE, m_Radious, 10, 10, Vector4(0, 1, 0, 1));
	m_CollBox->Initialize();


	//for (int i = 0; i < m_pModelData->m_pAllBoneList.size(); i++)
	//{
	//	GRAPHICENGINE::IGeoMetry_InterFace* pbox;
	//	GRAPHICENGINE::CreateGeometry(&pbox, GRAPHICENGINE::Geomerty_Sort::SPHERE, 1, 10, 10, Vector4(1, 0, 0, 1));
	//	pbox->Initialize();
	//	m_Bone.push_back(pbox);
	//}
	
}

void SKinned_Mesh::GetShaderResource()
{
	int cnt = m_pModelData->m_pMaterialList.size();
	if (m_AlbedoMapList.size() == 0 || m_NormalMapList.size() == 0 || m_MaskMapList.size() == 0 || m_ORMMapList.size() == 0 || m_EmissiveMapList.size() == 0)
	{
		m_AlbedoMapList.resize(cnt);
		m_NormalMapList.resize(cnt);
		m_MaskMapList.resize(cnt);
		m_ORMMapList.resize(cnt);
		m_EmissiveMapList.resize(cnt);
	}
	for (int i = 0; i < cnt; i++)
	{
		ID3D11ShaderResourceView* ptex;
		ptex = m_pModelData->m_pMaterialList[i]->GetShaderRes(Material::TextureSort::ALBEDO);
		// Albedo
		if (ptex != NULL)
		{
			m_AlbedoMapList[i] = ptex;
		}
		// Normal
		ptex = m_pModelData->m_pMaterialList[i]->GetShaderRes(Material::TextureSort::NORMAL);
		if (ptex != NULL)
		{
			m_NormalMapList[i] = ptex;
		}
		// MaskMap
		ptex = m_pModelData->m_pMaterialList[i]->GetShaderRes(Material::TextureSort::MASKMAP);
		if (ptex != NULL)
		{
			m_MaskMapList[i] = ptex;
		}
		// ORM
		ptex = m_pModelData->m_pMaterialList[i]->GetShaderRes(Material::TextureSort::ORM);
		if (ptex != NULL)
		{
			m_ORMMapList[i] = ptex;
		}
		// Emissive
		ptex = m_pModelData->m_pMaterialList[i]->GetShaderRes(Material::TextureSort::EMISSIVE);
		if (ptex != NULL)
		{
			m_EmissiveMapList[i] = ptex;
		}

	}
}

void SKinned_Mesh::ModelAniUpdate(float time, int anicount, int keycount)
{
	Matrix identityTM;

	for (int j = 0; j < m_pModelData->m_pAllBoneList.size(); j++)
	{
		ModelAnimationUpdate_Recur_Temp(anicount, m_pModelData->m_pAllBoneList[j], m_CurrTime, identityTM, m_AniBoneMatData, keycount);
	}

	for (int i = 0; i < m_pModelData->m_pAllBoneList.size(); i++)
	{
		m_CbDef.m_BoneTm[i] = m_pModelData->m_pAllBoneList[i]->GlobalBindingTm * m_AniBoneMatData[i];
	}

	//int xm = m_Bone.size();
	//for (int i = 0; i < xm; i++)
	//{
	//	m_Bone[i]->Update(m_AniBoneMatData[i] * m_UserTm);
	//}
}

void SKinned_Mesh::ModelAnimationUpdate_Recur_Temp(int anicount, Bone* pbone, float currtime, Matrix parenttm, vector<Matrix>& bonematdata, int nowframecount)
{
	/// 애니메이션이 하나라고 가정...

	int boneindex = pbone->BoneIndex;
	Matrix GlobalbindingTM = pbone->GlobalBindingTm;
	const OneFrameList* oneframe = m_pModelData->m_pAniDataList[anicount]->AnimationList[boneindex];

	int cnt = m_pModelData->m_pAniDataList[anicount]->TotalFrames;
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

	m_AniBoneMatData[boneindex] = anitm;
}
int SKinned_Mesh::CalAniFrame(float Time)
{
	float totaltime = m_pModelData->m_pAniDataList[m_NowAniCount]->TotalTime;
	int  totalframe = m_pModelData->m_pAniDataList[m_NowAniCount]->TotalFrames;
	float dtime = Time;
	float currtime = fmodf(dtime, totaltime);

	m_CurrTime = currtime;
	int framecount = currtime / (totaltime / totalframe);

	return framecount;
}

void SKinned_Mesh::SetPrimitiveData(int nodecount)
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

void SKinned_Mesh::SetTextrueData(int nodecount)
{
	if (m_AlbedoMapList.size() > 0)
	{
		int texcount = m_pFinalDatalist[nodecount]->m_pFbxData->m_TextureCount;

		// Albedo
		if (m_AlbedoMapList[texcount] != NULL)
			m_pDevConCopy->PSSetShaderResources(0, 1, &m_AlbedoMapList[texcount]);
		// Normal
		if (m_NormalMapList[texcount] != NULL)
			m_pDevConCopy->PSSetShaderResources(1, 1, &m_NormalMapList[texcount]);
		//MaskMap
		if (m_MaskMapList[texcount] != NULL)
			m_pDevConCopy->PSSetShaderResources(2, 1, &m_MaskMapList[texcount]);

		if (m_ORMMapList[texcount] != NULL)
			m_pDevConCopy->PSSetShaderResources(3, 1, &m_ORMMapList[texcount]);

		m_pDevConCopy->PSSetShaderResources(4, 1, &ShaderMgr::GetInstance()->m_pShadow);
		m_pDevConCopy->PSSetShaderResources(5, 1, &ShaderMgr::GetInstance()->m_pSkybox);
		m_pDevConCopy->PSSetShaderResources(6, 1, &ShaderMgr::GetInstance()->m_pSkybox_irradiance);

	}

	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_WRAP]);
	m_pDevConCopy->PSSetSamplers(1, 1, &D3DDevice::GetInstance()->m_pSampler[SS_BOARDER]);
	m_pDevConCopy->PSSetSamplers(2, 1, &D3DDevice::GetInstance()->m_pSampler[SS_CLAMP]);
}

void SKinned_Mesh::SetMaterialData(int nodecount)
{
	if (m_pFinalDatalist[nodecount]->m_pFbxData->m_pMaterial != NULL)
	{
		int texcount = m_pFinalDatalist[nodecount]->m_pFbxData->m_TextureCount;
		
		m_CbMat.Diffuse = m_pFinalDatalist[nodecount]->m_pFbxData->m_pMaterial->Diffuse;
		m_CbMat.Ambient = m_pFinalDatalist[nodecount]->m_pFbxData->m_pMaterial->Ambient;
		m_CbMat.Specular = m_pFinalDatalist[nodecount]->m_pFbxData->m_pMaterial->Specular;
		m_CbMat.TexOn = m_pModelData->m_pMaterialList[texcount]->m_TexOn;
		m_CbMat.NormalOn = m_pModelData->m_pMaterialList[texcount]->m_NorOn;
		m_CbMat.MaskOn = m_pModelData->m_pMaterialList[texcount]->m_MaskOn;
		m_CbMat.PBROn = m_pModelData->m_pMaterialList[texcount]->m_PBROn;
		//m_CbMat.PBROn = false;
		m_CbMat.SkinOn = m_AniOn;
	}
}

void SKinned_Mesh::SetMaterialData(GRAPHICENGINE::IMaterialInterface* pobj, string nodename, GRAPHICENGINE::IMaterialInterface::TextureSort sort)
{
	int size = m_pModelData->m_pMaterialList.size();
	for (int i = 0; i < size; i++)
	{
		if (nodename == m_pModelData->m_pMaterialList[i]->GetLinkNodename())
		{
			switch (sort)
			{
			case GRAPHICENGINE::IMaterialInterface::TextureSort::ALBEDO:
				m_pModelData->m_pMaterialList[i]->SetShaderRes(pobj->GetTexturePath(sort), sort);
				break;
			case GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL:
				m_pModelData->m_pMaterialList[i]->SetShaderRes(pobj->GetTexturePath(sort), sort);
				break;
			case GRAPHICENGINE::IMaterialInterface::TextureSort::ORM:
				m_pModelData->m_pMaterialList[i]->SetShaderRes(pobj->GetTexturePath(sort), sort);
				break;
			case GRAPHICENGINE::IMaterialInterface::TextureSort::MASKMAP:
				m_pModelData->m_pMaterialList[i]->SetShaderRes(pobj->GetTexturePath(sort), sort);
				break;
			case GRAPHICENGINE::IMaterialInterface::TextureSort::EMISSIVE:
				break;
			}
		}
	}
}

void SKinned_Mesh::SetEmissiveMapOnOff(bool b)
{

}

void SKinned_Mesh::AddEmissiveMap(string filename)
{

}

void SKinned_Mesh::ChangeApplyEmissiveMap(int index)
{

}

void SKinned_Mesh::SetDirectionLightData(GRAPHICENGINE::IDirectionLightInterface::IDirectrionLitData data)
{
	m_CbLight.direction = data.Direction;
	m_CbLight.diffuse = data.Diffuse;
	m_CbLight.ambient= data.Ambient;
}

void SKinned_Mesh::SetPointLightData(GRAPHICENGINE::IPointLightInterface::IPointLitData data)
{
	// 일단 Keep...
}

void SKinned_Mesh::SetConstantBuffer(int nodecount)
{
	m_CBFog = ShaderMgr::GetInstance()->m_Fog;
	m_ChannelView = ShaderMgr::GetInstance()->m_ChannelView;

	// 상수 버퍼 데이터 전달
	m_CbDef.m_AxisChange = m_pModelData->m_AxisChange;
	m_CbDef.mTm = m_pFinalDatalist[nodecount]->m_pFbxData->m_Worldm * m_UserTm;
	m_CbDef.mWorld = m_pFinalDatalist[nodecount]->m_pFbxData->m_Worldm;
	m_CbDef.mWV = m_CbDef.mTm * m_CbDef.mView;
	m_CbDef.mWVP = m_CbDef.mView * m_CbDef.mProj;
	m_CbDef.m_LightViewProj = ShaderMgr::GetInstance()->m_LightViewProjMat;

	// 상수 버퍼
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbDefBuf, &m_CbDef, sizeof(CBFBXDEF));
	// 조명 정보
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbLightBuf, &m_CbLight, sizeof(CBLIGHT));
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbMatBuf, &m_CbMat, sizeof(CBFBXMAT));
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbFogBuf, &m_CBFog, sizeof(CBFOG));
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbChannelViewBuf, &m_ChannelView, sizeof(CHANNELVIEW));

	//기본 셰이더 설정.
	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	//기본 셰이더 상수버퍼 설정.
	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pCbDefBuf);
	m_pDevConCopy->VSSetConstantBuffers(1, 1, &m_pCbMatBuf);
	m_pDevConCopy->VSSetConstantBuffers(2, 1, &m_pCbLightBuf);
	m_pDevConCopy->VSSetConstantBuffers(4, 1, &m_pCbFogBuf);
	m_pDevConCopy->VSSetConstantBuffers(5, 1, &m_pCbChannelViewBuf);

	// 픽셀 셰이더 상수 버퍼 설정.
	m_pDevConCopy->PSSetConstantBuffers(0, 1, &m_pCbDefBuf);
	m_pDevConCopy->PSSetConstantBuffers(1, 1, &m_pCbMatBuf);
	m_pDevConCopy->PSSetConstantBuffers(2, 1, &m_pCbLightBuf);
	m_pDevConCopy->PSSetConstantBuffers(4, 1, &m_pCbFogBuf);
	m_pDevConCopy->PSSetConstantBuffers(5, 1, &m_pCbChannelViewBuf);
}

void SKinned_Mesh::SetConstantBuffer_Depth(int nodecount, Matrix shadow)
{
	// 상수 버퍼 데이터 전달
	m_CbDef.m_AxisChange = m_pModelData->m_AxisChange;
	m_CbDef.mTm = m_pFinalDatalist[nodecount]->m_pFbxData->m_Worldm * m_UserTm;
	m_CbDef.mWorld = m_pFinalDatalist[nodecount]->m_pFbxData->m_Worldm;
	m_CbDef.mWV = m_CbDef.mTm * m_CbDef.mView;
	m_CbDef.mWVP = shadow;
	m_CbDef.m_LightViewProj = shadow;

	// 상수 버퍼
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbDefBuf, &m_CbDef, sizeof(CBFBXDEF));

	// 조명 정보
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbLightBuf, &m_CbLight, sizeof(CBLIGHT));

	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbMatBuf, &m_CbMat, sizeof(CBFBXMAT));

	//기본 셰이더 설정.
	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pPsDepth, nullptr, 0);

	//기본 셰이더 상수버퍼 설정.
	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pCbDefBuf);
	m_pDevConCopy->VSSetConstantBuffers(1, 1, &m_pCbMatBuf);
	m_pDevConCopy->VSSetConstantBuffers(2, 1, &m_pCbLightBuf);

	// 픽셀 셰이더 상수 버퍼 설정.
	m_pDevConCopy->PSSetConstantBuffers(0, 1, &m_pCbDefBuf);
	m_pDevConCopy->PSSetConstantBuffers(1, 1, &m_pCbMatBuf);
	m_pDevConCopy->PSSetConstantBuffers(2, 1, &m_pCbLightBuf);
}

void SKinned_Mesh::Draw(int nodecount)
{
	GRAPHICENGINE::RenderQueueSort sort = m_pFinalDatalist[nodecount]->m_QueueSrot;
	switch (sort)
	{
	case GRAPHICENGINE::RenderQueueSort::SORT_BACKGROUND:
		break;
	case GRAPHICENGINE::RenderQueueSort::SORT_OPAQUE:
		break;
	case GRAPHICENGINE::RenderQueueSort::SORT_ALPHATEST:

		/// 악마 전용... 외부로 Render Option 전환 필요
		if (m_pFinalDatalist[nodecount]->m_pFbxData->m_NodeName == "wing")
		{
			if (m_ModelRenderState == GRAPHICENGINE::RenderState::WIREFRAME)
			{

			}
			else
			{
				D3DDevice::GetInstance()->ChangeRenderState(GRAPHICENGINE::RenderState::SOLID);
				m_pDevConCopy->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_ON], NULL, 0xFFFFFFFF);
				m_pDevConCopy->DrawIndexed(m_pFinalDatalist[nodecount]->m_pFbxData->m_pVertexList.size(), 0, 0);
				m_pDevConCopy->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_OFF], NULL, 0xFFFFFFFF);
				D3DDevice::GetInstance()->ChangeRenderState(GRAPHICENGINE::RenderState::CULLBACK);
			}
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
			m_pDevConCopy->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_ON], NULL, 0xFFFFFFFF);
			m_pDevConCopy->DrawIndexed(m_pFinalDatalist[nodecount]->m_pFbxData->m_pVertexList.size(), 0, 0);
			m_pDevConCopy->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_OFF], NULL, 0xFFFFFFFF);
		}

		break;
	case GRAPHICENGINE::RenderQueueSort::SORT_OPAQUELAST:
		break;
	case GRAPHICENGINE::RenderQueueSort::SORT_TRANSPARENT:
		m_pDevConCopy->DrawIndexed(m_pFinalDatalist[nodecount]->m_pFbxData->m_pVertexList.size(), 0, 0);
		break;
	case GRAPHICENGINE::RenderQueueSort::SORT_OVERLAY:
		break;
	case GRAPHICENGINE::RenderQueueSort::SORT_UI:
		break;
	}

	//int xm = m_Bone.size();
	//for (int i = 0; i < xm; i++)
	//{
	//	m_Bone[i]->Draw();
	//}
}

///override//////////////////////////////////////////////////////////////////////////

void SKinned_Mesh::FbxModelUpdate(float time)
{
	if (m_AniOn == true && m_AniOnOff == true)
	{
		m_NowFrameCount = CalAniFrame(m_AniTime);

		if (m_NowFrameCount <= m_pModelData->m_pAniDataList[m_NowAniCount]->TotalFrames - 2 && m_Aniloop == false)
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

void SKinned_Mesh::FbxModelDraw(float time, int nodecount, RenderPath path, Matrix shadow)
{
	// 노드 카운트 설정
	m_NowNodeCount = nodecount;
	// 카메라 행렬 정보 전달
	m_CbDef.mView = D3DDevice::GetInstance()->m_View;
	m_CbDef.mProj = D3DDevice::GetInstance()->m_Proj;

	// ViewMatrix inv.
	XMMATRIX invView = XMMatrixInverse(NULL, m_CbDef.mView);
	Vector3 Cam = XMVector3TransformCoord(XMVectorSet(0, 0, 0, 1), invView);
	m_CbDef.CamWorldPos = Cam;

	// 외부 렌더 큐에서 돌려준다...
	SetPrimitiveData(nodecount);
	SetTextrueData(nodecount);
	SetMaterialData(nodecount);
	switch (path)
	{
	case GRAPHICENGINE::IModelInterface::RenderPath::Default:
		SetConstantBuffer(nodecount);
		break;

	case GRAPHICENGINE::IModelInterface::RenderPath::Depth:
		SetConstantBuffer_Depth(nodecount, shadow);
		break;
	}

	switch (m_ModelRenderState)
	{
	case GRAPHICENGINE::RenderState::CULLBACK:
		D3DDevice::GetInstance()->ChangeRenderState(GRAPHICENGINE::RenderState::CULLBACK);
		break;
	case GRAPHICENGINE::RenderState::CULLFRONT:
		D3DDevice::GetInstance()->ChangeRenderState(GRAPHICENGINE::RenderState::CULLFRONT);
		break;
	case GRAPHICENGINE::RenderState::WIREFRAME:
		D3DDevice::GetInstance()->ChangeRenderState(GRAPHICENGINE::RenderState::WIREFRAME);
		break;
	case GRAPHICENGINE::RenderState::SOLID:
		D3DDevice::GetInstance()->ChangeRenderState(GRAPHICENGINE::RenderState::SOLID);
		break;
	}
	Draw(nodecount);

	//m_CollBox->Draw();
}

void SKinned_Mesh::FbxModelInfo()
{

}

void SKinned_Mesh::ReleaseFbxModel()
{
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

void SKinned_Mesh::SetUserMatrix(Matrix usertm)
{
	m_UserTm = usertm;
}

Matrix SKinned_Mesh::GetUserMatrix()
{
	return m_UserTm;
}

std::string SKinned_Mesh::GetModelName()
{
	return m_pModelData->m_ModelName;
}

std::string SKinned_Mesh::GetNodeName(int nodecount)
{
	return m_pFinalDatalist[nodecount]->m_pFbxData->m_NodeName;
}

int SKinned_Mesh::GetModelNodeCount()
{
	return m_pFinalDatalist.size();
}

GRAPHICENGINE::RenderQueueSort SKinned_Mesh::GetModelNodeRendersort(int nodecount)
{
	GRAPHICENGINE::RenderQueueSort sort = m_pFinalDatalist[nodecount]->m_QueueSrot;

	return sort;
}

void SKinned_Mesh::SetModelNodeRenderQueuesort(int nodecount, GRAPHICENGINE::RenderQueueSort sort)
{
	m_pFinalDatalist[nodecount]->m_QueueSrot = sort;
}

void SKinned_Mesh::SetDrawScreen(bool draw)
{
	m_Draw = draw;
}

bool SKinned_Mesh::GetDrawScreen()
{
	return m_Draw;
}

void SKinned_Mesh::SetModelRenderState(GRAPHICENGINE::RenderState state)
{
	switch (state)
	{
	case GRAPHICENGINE::RenderState::CULLBACK:
		m_ModelRenderState = GRAPHICENGINE::RenderState::CULLBACK;
		break;
	case GRAPHICENGINE::RenderState::CULLFRONT:
		m_ModelRenderState = GRAPHICENGINE::RenderState::CULLFRONT;
		break;
	case GRAPHICENGINE::RenderState::WIREFRAME:
		m_ModelRenderState = GRAPHICENGINE::RenderState::WIREFRAME;
		break;
	case GRAPHICENGINE::RenderState::SOLID:
		m_ModelRenderState = GRAPHICENGINE::RenderState::SOLID;
		break;
	}
}

DirectX::SimpleMath::Vector3 SKinned_Mesh::GetWorldModelPosition()
{
	Matrix NowMatrix = m_pFinalDatalist[m_NowNodeCount]->m_pFbxData->m_Worldm * m_UserTm;

	XMVECTOR Pos, rot, scl;

	XMMatrixDecompose(&scl, &rot, &Pos, NowMatrix);

	return Pos;
}

float SKinned_Mesh::GetCollSphereRadious()
{
	return m_Radious;
}

GRAPHICENGINE::IModelInterface::ModelSort SKinned_Mesh::GetModelSort()
{
	if (m_pModelData->m_MeshFilterSort == MeshFilter::eMeshfiltersort::StaticMesh)
	{
		return IModelInterface::ModelSort::STATIC_MESH;
	}
	else
	{
		return IModelInterface::ModelSort::SKINNED_MESH;
	}
}

void SKinned_Mesh::SetAnimationSpeed(float speed)
{
	m_AnimationSpeed = speed;
}

void SKinned_Mesh::ChangeAnimation(int count)
{
	m_NowAniCount = count;
	// 현재 애니메이션 프레임을 0으로 만들어 준다
	m_AniTime = 0;
}

void SKinned_Mesh::SetAnimationLoop(bool loop)
{
	m_Aniloop = loop;
}

float SKinned_Mesh::GetAniTotalTime()
{
	float totaltime = m_pModelData->m_pAniDataList[m_NowAniCount]->TotalTime;

	return totaltime;
}

bool SKinned_Mesh::GetAniEndTimeing()
{
	if (m_NowFrameCount >= m_pModelData->m_pAniDataList[m_NowAniCount]->TotalFrames - 2)
	{
		return true;
	}
	return false;
}

int SKinned_Mesh::GetAniTotalKeyCount()
{
	return m_pModelData->m_pAniDataList[m_NowAniCount]->TotalFrames;
}

int SKinned_Mesh::GetAniNowKeyCount()
{
	return m_NowFrameCount;
}

void SKinned_Mesh::SetNowKeyCount(int key)
{
	m_NowFrameCount = key;
}

void SKinned_Mesh::SetAniOn()
{
	m_AniOnOff = true;
}

void SKinned_Mesh::SetAniOff()
{
	m_AniOnOff = false;
}

Matrix SKinned_Mesh::GetHiearchyMatrixByName(string name)
{
	Matrix boneanimat;
	int cnt = m_pModelData->m_pAllBoneList.size();
	for (int i = 0; i < cnt; i++)
	{
		if (m_pModelData->m_pAllBoneList[i]->m_Name == name)
		{
			boneanimat = m_AniBoneMatData[i] * m_pFinalDatalist[m_NowNodeCount]->m_pFbxData->m_Worldm;
			break;
		}
	}
	return boneanimat;
}

std::vector<std::string> SKinned_Mesh::GetBoneOffSetName()
{
	vector<string> tmp;
	int cnt = m_pModelData->m_pAllBoneList.size();
	tmp.resize(cnt);
	for (int i = 0; i < cnt; i++)
	{
		tmp[i] =  m_pModelData->m_pAllBoneList[i]->m_Name;
	}

	return tmp;
}

