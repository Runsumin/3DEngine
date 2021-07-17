#include "Static_Mesh.h"
Static_Mesh::Static_Mesh()
	:m_UserTm(Matrix::Identity), m_Trans(Matrix::Identity), m_Rot(Matrix::Identity), m_Scale(Matrix::Identity),
	m_vPos(Vector3::Zero), m_vScl(Vector3::Zero), m_vRot(Vector3::Zero), m_LightVP(Matrix::Identity), m_NowNodeCount(0),
	m_EmssiveprevState(false)
{
	// 장치 정보 받아오기
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();
	m_pShader = ShaderMgr::GetInstance()->m_pStatic;
	m_pPsDepth = ShaderMgr::GetInstance()->m_pPsDepth;
	m_Draw = true;
	m_ModelRenderState = GRAPHICENGINE::RenderState::CULLBACK;
}

Static_Mesh::~Static_Mesh()
{

}

bool Static_Mesh::SetModelData(Fbx_MeshData* pdata)
{
	m_pModelData = pdata;
	if (m_pModelData == nullptr)
		return false;
	else
		return true;
}

void Static_Mesh::InitFbxModel()
{
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
}

void Static_Mesh::InitModelTexture()
{
	GetShaderResource();
}

void Static_Mesh::CreateHWBuffer()
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
	m_Radious = radious1 + radious2;
	GRAPHICENGINE::CreateGeometry(&m_CollBox, GRAPHICENGINE::Geomerty_Sort::SPHERE, m_Radious, 10, 10, Vector4(0, 1, 0, 1));
	m_CollBox->Initialize();
}

void Static_Mesh::GetShaderResource()
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


void Static_Mesh::SetPrimitiveData(int nodecount)
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

void Static_Mesh::SetTextrueData(int nodecount)
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
		// ORM
		if (m_ORMMapList[texcount] != NULL)
			m_pDevConCopy->PSSetShaderResources(3, 1, &m_ORMMapList[texcount]);

		if (m_pModelData->m_pMaterialList[texcount]->m_EmissiveOn == true &&
			m_EmssiveprevState == true)
		{
			// Emissive
			if (m_EmissiveMapList[m_EmissiveMapCount] != NULL)
				m_pDevConCopy->PSSetShaderResources(4, 1, &m_EmissiveMapList[m_EmissiveMapCount]);
		}

		// SkyBox
		m_pDevConCopy->PSSetShaderResources(5, 1, &ShaderMgr::GetInstance()->m_pSkybox);

		// Shadow Map
		m_pDevConCopy->PSSetShaderResources(6, 1, &ShaderMgr::GetInstance()->m_pShadow);
		m_pDevConCopy->PSSetShaderResources(7, 1, &ShaderMgr::GetInstance()->m_pSkybox_irradiance);
	}

	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_WRAP]);
	m_pDevConCopy->PSSetSamplers(1, 1, &D3DDevice::GetInstance()->m_pSampler[SS_BOARDER]);
	m_pDevConCopy->PSSetSamplers(2, 1, &D3DDevice::GetInstance()->m_pSampler[SS_CLAMP]);
}

void Static_Mesh::SetMaterialData(int nodecount)
{
	if (m_pFinalDatalist[nodecount]->m_pFbxData->m_pMaterial != NULL)
	{
		int texcount = m_pFinalDatalist[nodecount]->m_pFbxData->m_TextureCount;
		int norcount = m_pFinalDatalist[nodecount]->m_pFbxData->m_NormalCount;
		int maskcount = m_pFinalDatalist[nodecount]->m_pFbxData->m_MaskMapCount;

		m_CbMat.Diffuse = m_pFinalDatalist[nodecount]->m_pFbxData->m_pMaterial->Diffuse;
		m_CbMat.Ambient = m_pFinalDatalist[nodecount]->m_pFbxData->m_pMaterial->Ambient;
		m_CbMat.Specular = m_pFinalDatalist[nodecount]->m_pFbxData->m_pMaterial->Specular;
		m_CbMat.TexOn = m_pModelData->m_pMaterialList[texcount]->m_TexOn;
		m_CbMat.NormalOn = m_pModelData->m_pMaterialList[texcount]->m_NorOn;
		m_CbMat.MaskOn = m_pModelData->m_pMaterialList[texcount]->m_MaskOn;
		m_CbMat.PBROn = m_pModelData->m_pMaterialList[texcount]->m_PBROn;
		//m_CbMat.EmissiveOn = m_pModelData->m_pMaterialList[texcount]->m_EmissiveOn;
		if (m_pModelData->m_pMaterialList[texcount]->m_EmissiveOn == true &&
			m_EmssiveprevState == true)
		{
			m_CbMat.EmissiveOn = true;
		}
		else
		{
			m_CbMat.EmissiveOn = false;
		}
		m_CbMat.SkinOn = false;
	}

}

void Static_Mesh::SetMaterialData(GRAPHICENGINE::IMaterialInterface* pobj, string nodename, GRAPHICENGINE::IMaterialInterface::TextureSort sort)
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
				m_pModelData->m_pMaterialList[i]->SetShaderRes(pobj->GetTexturePath(sort), sort);
				break;
			}
		}
	}
}

void Static_Mesh::SetEmissiveMapOnOff(bool b)
{
	//int cnt = m_pModelData->m_pMaterialList.size();

	m_EmssiveprevState = b;
	//if (b == true)
	//{
	//	for (int i = 0; i < cnt; i++)
	//	{
	//		if (m_pModelData->m_pMaterialList[i]->m_EmissiveOn == false)
	//		{
	//			m_pModelData->m_pMaterialList[i]->m_EmissiveOn = true;
	//		}
	//	}
	//}
	//else if (b == false)
	//{
	//	for (int i = 0; i < cnt; i++)
	//	{
	//		if (m_pModelData->m_pMaterialList[i]->m_EmissiveOn == true)
	//		{
	//			m_pModelData->m_pMaterialList[i]->m_EmissiveOn = false;
	//		}
	//	}
	//}
}

void Static_Mesh::AddEmissiveMap(string filename)
{
	HRESULT hr;
	ID3D11ShaderResourceView* ptex = nullptr;

	hr = D3DDevice::GetInstance()->LoadTexture(filename, &ptex);

	m_EmissiveMapList.push_back(ptex);
}

void Static_Mesh::ChangeApplyEmissiveMap(int index)
{
	int cnt = m_EmissiveMapList.size();

	if (index > cnt)
	{
		
	}
	else
	{
		m_EmissiveMapCount = index;
	}
}

void Static_Mesh::SetDirectionLightData(GRAPHICENGINE::IDirectionLightInterface::IDirectrionLitData data)
{
	m_CbLight.direction = data.Direction;
	m_CbLight.diffuse = data.Diffuse;
	m_CbLight.ambient = data.Ambient;
}

void Static_Mesh::SetPointLightData(GRAPHICENGINE::IPointLightInterface::IPointLitData data)
{

}

void Static_Mesh::SetConstantBuffer(int nodecount)
{
	m_CBFog = ShaderMgr::GetInstance()->m_Fog;
	m_ChannelView = ShaderMgr::GetInstance()->m_ChannelView;

	// 상수 버퍼 데이터 전달
	m_CbDef.m_AxisChange = m_pModelData->m_AxisChange;
	m_CbDef.mTm = m_pFinalDatalist[nodecount]->m_pFbxData->m_Worldm * m_UserTm * m_pModelData->m_AxisChange;
	m_CbDef.mWorld = m_pFinalDatalist[nodecount]->m_pFbxData->m_Worldm;
	m_CbDef.mWV = m_CbDef.mTm * m_CbDef.mView;
	m_CbDef.mWVP = m_CbDef.mView * m_CbDef.mProj;
	m_CbDef.m_LightViewProj = ShaderMgr::GetInstance()->m_LightViewProjMat;


	// 상수 버퍼
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbDefBuf, &m_CbDef, sizeof(CBFBXDEF));
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

void Static_Mesh::SetConstantBuffer_Depth(int nodecount, Matrix shadow)
{
	// 상수 버퍼 데이터 전달
	m_CbDef.m_AxisChange = m_pModelData->m_AxisChange;
	m_CbDef.mTm = m_pFinalDatalist[nodecount]->m_pFbxData->m_Worldm * m_UserTm;
	m_CbDef.mWorld = m_pFinalDatalist[nodecount]->m_pFbxData->m_Worldm;
	m_CbDef.mWV = m_CbDef.mTm * m_CbDef.mView;
	m_CbDef.mWVP = shadow;
	m_CbDef.m_LightViewProj = shadow;

	/// 임시로 최종 행렬에 shadow 행렬 대입

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

void Static_Mesh::Draw(int nodecount)
{
	GRAPHICENGINE::RenderQueueSort sort = m_pFinalDatalist[nodecount]->m_QueueSrot;
	switch (sort)
	{
	case GRAPHICENGINE::RenderQueueSort::SORT_BACKGROUND:
		break;
	case GRAPHICENGINE::RenderQueueSort::SORT_OPAQUE:
		break;
	case GRAPHICENGINE::RenderQueueSort::SORT_ALPHATEST:
		m_pDevConCopy->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_ON], NULL, 0xFFFFFFFF);
		m_pDevConCopy->DrawIndexed(m_pFinalDatalist[nodecount]->m_pFbxData->m_pVertexList.size(), 0, 0);
		m_pDevConCopy->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_OFF], NULL, 0xFFFFFFFF);
		break;
	case GRAPHICENGINE::RenderQueueSort::SORT_OPAQUELAST:
		break;
	case GRAPHICENGINE::RenderQueueSort::SORT_TRANSPARENT:

	//	D3DDevice::GetInstance()->ChangeRenderState(GRAPHICENGINE::RenderState::SOLID);
		m_pDevConCopy->DrawIndexed(m_pFinalDatalist[nodecount]->m_pFbxData->m_pVertexList.size(), 0, 0);
	//	D3DDevice::GetInstance()->ChangeRenderState(GRAPHICENGINE::RenderState::SOLID);
		break;
	case GRAPHICENGINE::RenderQueueSort::SORT_OVERLAY:
		break;
	case GRAPHICENGINE::RenderQueueSort::SORT_UI:
		break;
	}
}

///override//////////////////////////////////////////////////////////////////////////

void Static_Mesh::FbxModelUpdate(float time)
{
	// 카메라 행렬 정보 전달
	m_CbDef.mView = D3DDevice::GetInstance()->m_View;
	m_CbDef.mProj = D3DDevice::GetInstance()->m_Proj;

	// ViewMatrix inv.
	XMMATRIX invView = XMMatrixInverse(NULL, m_CbDef.mView);
	Vector3 Cam = XMVector3TransformCoord(XMVectorSet(0, 0, 0, 1), invView);
	m_CbDef.CamWorldPos = Cam;

	//m_CollBox->Update(m_UserTm);
}

void Static_Mesh::FbxModelDraw(float time, int nodecount, RenderPath path, Matrix shadow)
{
	// 노드 카운트 설정
	m_NowNodeCount = nodecount;
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

void Static_Mesh::FbxModelInfo()
{

}

void Static_Mesh::ReleaseFbxModel()
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
}

void Static_Mesh::SetUserMatrix(Matrix usertm)
{
	m_UserTm = usertm;
}

Matrix Static_Mesh::GetUserMatrix()
{
	return m_UserTm;
}

std::string Static_Mesh::GetModelName()
{
	return m_pModelData->m_ModelName;
}

std::string Static_Mesh::GetNodeName(int nodecount)
{
	return m_pFinalDatalist[nodecount]->m_pFbxData->m_NodeName;
}

int Static_Mesh::GetModelNodeCount()
{
	return m_pFinalDatalist.size();
}

GRAPHICENGINE::RenderQueueSort Static_Mesh::GetModelNodeRendersort(int nodecount)
{

	GRAPHICENGINE::RenderQueueSort sort = m_pFinalDatalist[nodecount]->m_QueueSrot;

	return sort;
}

void Static_Mesh::SetModelNodeRenderQueuesort(int nodecount, GRAPHICENGINE::RenderQueueSort sort)
{
	m_pFinalDatalist[nodecount]->m_QueueSrot = sort;
}

void Static_Mesh::SetDrawScreen(bool draw)
{
	m_Draw = draw;
}

bool Static_Mesh::GetDrawScreen()
{
	return m_Draw;
}

void Static_Mesh::SetModelRenderState(GRAPHICENGINE::RenderState state)
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

DirectX::SimpleMath::Vector3 Static_Mesh::GetWorldModelPosition()
{
	Matrix NowMatrix = m_UserTm;

	XMVECTOR Pos, rot, scl;

	XMMatrixDecompose(&scl, &rot, &Pos, NowMatrix);

	return Pos;
}

GRAPHICENGINE::IModelInterface::ModelSort Static_Mesh::GetModelSort()
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
///o
void Static_Mesh::SetAnimationSpeed(float speed)
{
	// 구현X
}

void Static_Mesh::ChangeAnimation(int count)
{
	// 구현X
}

float Static_Mesh::GetAniTotalTime()
{
	// 구현X
	return 0;
}

bool Static_Mesh::GetAniEndTimeing()
{
	// 구현X
	return false;
}

int Static_Mesh::GetAniTotalKeyCount()
{
	// 구현X
	return 0;
}

int Static_Mesh::GetAniNowKeyCount()
{
	// 구현X
	return 0;
}

void Static_Mesh::SetNowKeyCount(int key)
{
	// 구현X
}

void Static_Mesh::SetAniOn()
{
	// 구현X
}

void Static_Mesh::SetAniOff()
{
	// 구현X
}

void Static_Mesh::SetAnimationLoop(bool loop)
{
	// 구현X
}

Matrix Static_Mesh::GetHiearchyMatrixByName(string name)
{
	return Matrix::Identity;
}

std::vector<std::string> Static_Mesh::GetBoneOffSetName()
{
	vector<string> tmp;

	return tmp;
}

float Static_Mesh::GetCollSphereRadious()
{
	return m_Radious;
}

