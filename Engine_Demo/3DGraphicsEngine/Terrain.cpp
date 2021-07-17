#include "Terrain.h"
#include "ShaderMgr.h"
Terrain::Terrain(string filename, float width, float height, float slice)
{
	// 장치 정보 받아오기
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();
	m_Filename = filename;
	m_Width = width;
	m_Height = height;
	m_Slice = slice;
}

Terrain::~Terrain()
{

}

void Terrain::InitTerrain()
{
	_VTXTERRAIN  MeshTerrain[] = {
	{-m_Width, 0.01f, m_Height,  0.0, 1.0f, 0.0,   0.0,  0.0},  //R,G,B,A
	{ m_Width, 0.01f, m_Height,  0.0, 1.0f, 0.0,  m_Slice,  0.0 },
	{-m_Width, 0.01f,-m_Height,  0.0, 1.0f, 0.0,   0.0, m_Slice },
	{ m_Width, 0.01f,-m_Height,  0.0, 1.0f, 0.0,  m_Slice, m_Slice },
	};

	HRESULT res = S_OK;

	res = D3DDevice::GetInstance()->CreateVB(MeshTerrain, sizeof(MeshTerrain), &m_pVb);

	m_pShader = new Shader();

	m_pShader->ShaderLoad(L"../shader/Terrain.fx", m_pDevCopy);

	/// 각 셰이더마다 다르게 정점 레이아웃 설정 가능(모델 & 상황포함)
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION",	   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		   0, DXGI_FORMAT_R32G32B32_FLOAT,	  0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	   0, DXGI_FORMAT_R32G32_FLOAT,		  0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);
	m_pShader->CreateVBLayOut(layout, numElements, m_pDevCopy);

	// 뎁스 버퍼 저장용 픽셀 셰이더 로드
	D3DDevice::GetInstance()->PixelShaderLoad(L"../shader/Terrain.fx", "PS_DEPTH", "ps_5_0", &m_pPsDepth);


	ZeroMemory(&m_cbTerrain, sizeof(CBTERRAIN));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBTERRAIN), &m_cbTerrain, &m_pTerrainbuf, m_pDevCopy);
	ZeroMemory(&m_CbLight, sizeof(CBLIGHT));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBLIGHT), &m_CbLight, &m_pCbLightBuf, m_pDevCopy);
	ZeroMemory(&m_State, sizeof(CBState));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBState), &m_State, &m_pStateBuf, m_pDevCopy);
	ZeroMemory(&m_CBFog, sizeof(CBFOG));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBFOG), &m_CBFog, &m_pCbFogBuf, m_pDevCopy);

	// 셰이더 셋팅
	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pTerrainbuf);

	D3DDevice::GetInstance()->LoadTexture(m_Filename, &g_pTexTerrain);
}

void Terrain::Update()
{
	m_cbTerrain.mView = D3DDevice::GetInstance()->m_View;
	m_cbTerrain.mProj = D3DDevice::GetInstance()->m_Proj;
	m_CbLight = D3DDevice::GetInstance()->GiveDirectionLightData();

	XMMATRIX mTm = XMMatrixIdentity();
	
	m_cbTerrain.mWorld = mTm;

	Matrix m = Matrix::Identity;
	m._11 = 0.5f;	m._22 = -0.5f;	 m._33 = 1.0f;
	m._41 = 0.5f;	m._42 = 0.5f;   m._43 = 1.0f;

	m_cbTerrain.mTextm = m;

}

void Terrain::Render(GRAPHICENGINE::ITerrain_Interface::RenderPath path, Matrix shadow, bool b)
{
	m_CBFog = ShaderMgr::GetInstance()->m_Fog;
	m_State.ShadowOnOff = b;

	UINT stride = sizeof(_VTXTERRAIN);
	UINT offset = 0;
	m_pDevConCopy->IASetVertexBuffers(0, 1, &m_pVb, &stride, &offset);
	m_pDevConCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDevConCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	//최종 행렬 결합.
	m_cbTerrain.mWVP = m_cbTerrain.mWorld * m_cbTerrain.mView * m_cbTerrain.mProj;
	m_cbTerrain.mLightVP = shadow;
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pTerrainbuf, &m_cbTerrain, sizeof(CBTERRAIN));
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbLightBuf, &m_CbLight, sizeof(CBLIGHT));
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pStateBuf, &m_State, sizeof(CBState));
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbFogBuf, &m_CBFog, sizeof(CBFOG));

	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pTerrainbuf);
	m_pDevConCopy->VSSetConstantBuffers(1, 1, &m_pStateBuf);
	m_pDevConCopy->VSSetConstantBuffers(2, 1, &m_pCbLightBuf);
	m_pDevConCopy->VSSetConstantBuffers(3, 1, &m_pCbFogBuf);

	m_pDevConCopy->PSSetConstantBuffers(0, 1, &m_pTerrainbuf);
	m_pDevConCopy->PSSetConstantBuffers(1, 1, &m_pStateBuf);
	m_pDevConCopy->PSSetConstantBuffers(2, 1, &m_pCbLightBuf);
	m_pDevConCopy->PSSetConstantBuffers(3, 1, &m_pCbFogBuf);

	switch (path)
	{
	case GRAPHICENGINE::ITerrain_Interface::RenderPath::Default:

		//기본 셰이더 설정.
		m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
		m_pDevConCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

		m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_WRAP]);
		m_pDevConCopy->PSSetSamplers(1, 1, &D3DDevice::GetInstance()->m_pSampler[SS_CLAMP]);
		m_pDevConCopy->PSSetShaderResources(0, 1, &g_pTexTerrain);	//PS 에 텍스처 설정.
		m_pDevConCopy->PSSetShaderResources(1, 1, &ShaderMgr::GetInstance()->m_pShadow);	//PS 에 텍스처 설정.
		break;
	case GRAPHICENGINE::ITerrain_Interface::RenderPath::Depth:

		//기본 셰이더 설정.
		m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
		m_pDevConCopy->PSSetShader(m_pPsDepth, nullptr, 0);
		break;
	case GRAPHICENGINE::ITerrain_Interface::RenderPath::Shadow:
		break;
	default:
		break;
	}

	m_pDevConCopy->Draw(4, 0);

}

void Terrain::Release()
{

}

void Terrain::TerrainDraw_Depth(Matrix lightvpmat)
{

}
