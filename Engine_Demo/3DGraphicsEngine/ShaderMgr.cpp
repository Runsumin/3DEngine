#include "ShaderMgr.h"
ShaderMgr* ShaderMgr::m_Instance = nullptr;

ShaderMgr* ShaderMgr::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new ShaderMgr();
	}

	return m_Instance;
}

ShaderMgr::ShaderMgr()
{
	// 장치 정보 받아오기
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();
	ZeroMemory(&m_ChannelView, sizeof(CHANNELVIEW));
}

ShaderMgr::~ShaderMgr()
{

}

void ShaderMgr::InitShaderMgr()
{
	/// 모델에 사용할 셰이더 초기화
	/// Static Mesh
	m_pStatic = new Shader();

	m_pStatic->ShaderLoad(L"../shader/Static_Mesh.fx", m_pDevCopy);

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
		//{ "INSTANCEPOS",   0, DXGI_FORMAT_R32G32B32_FLOAT,		 0, 92, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	UINT numElements = ARRAYSIZE(layout);

	m_pStatic->CreateVBLayOut(layout, numElements, m_pDevCopy);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Skinned Mesh
	m_pSkinned = new Shader();

	m_pSkinned->ShaderLoad(L"../shader/Skinned_Mesh.fx", m_pDevCopy);

	D3D11_INPUT_ELEMENT_DESC layout2[] =
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

	UINT numElements2 = ARRAYSIZE(layout2);

	m_pSkinned->CreateVBLayOut(layout2, numElements2, m_pDevCopy);

	// 뎁스 버퍼 저장용 픽셀 셰이더 로드
	D3DDevice::GetInstance()->PixelShaderLoad(L"../shader/Skinned_Mesh.fx", "PS_DEPTH", "ps_5_0", &m_pPsDepth);
	// 그림자 드로우용 픽셀 셰이더 로드
	D3DDevice::GetInstance()->PixelShaderLoad(L"../shader/Skinned_Mesh.fx", "PS_SHADOW", "ps_5_0", &m_pPsShadow);
	// 뎁스 버퍼 저장용 픽셀 셰이더 로드
	D3DDevice::GetInstance()->PixelShaderLoad(L"../shader/Static_Mesh.fx", "PS_DEPTH", "ps_5_0", &m_pPsDepth_Static);
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	/// UI
	m_pUI = new Shader();

	m_pUI->ShaderLoad(L"../shader/UI.fx", m_pDevCopy);

	D3D11_INPUT_ELEMENT_DESC layout3[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements3 = ARRAYSIZE(layout3);

	m_pUI->CreateVBLayOut(layout3, numElements3, m_pDevCopy);


	// 뎁스 버퍼 저장용 픽셀 셰이더 로드
	D3DDevice::GetInstance()->PixelShaderLoad(L"../shader/UI.fx", "PS_DEPTH", "ps_5_0", &m_pPsDepth_UI);
	D3DDevice::GetInstance()->PixelShaderLoad(L"../shader/UI.fx", "PS_BLOOM", "ps_5_0", &m_pPsBloom);
	D3DDevice::GetInstance()->PixelShaderLoad(L"../shader/UI.fx", "PS_FINAL", "ps_5_0", &m_pPsFinal);
	D3DDevice::GetInstance()->PixelShaderLoad(L"../shader/UI.fx", "BLUR_ACCUMLATE", "ps_5_0", &m_pPsSampling);
	D3DDevice::GetInstance()->PixelShaderLoad(L"../shader/UI.fx", "PS_BLUR", "ps_5_0", &m_pPsBlur);

}

void ShaderMgr::ReleaseShaderMgr()
{
	m_pStatic->ShaderRelease();
	m_pSkinned->ShaderRelease();
	m_pUI->ShaderRelease();

	SAFE_RELEASE(m_pPsDepth);
	SAFE_RELEASE(m_pPsDepth_UI);
}
