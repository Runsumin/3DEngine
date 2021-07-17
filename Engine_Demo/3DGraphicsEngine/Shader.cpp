#include "Shader.h"

Shader::Shader()
	:m_pVS(nullptr), m_pPS(nullptr), m_pVSCode(nullptr), m_pVBLayout(nullptr)
{

}

Shader::~Shader()
{
	ShaderRelease();
}

HRESULT Shader::ShaderLoad(const TCHAR* fxname, ID3D11Device* pDevice)
{
	// 일단 기본 흐름은 이렇게 따라간다...
	HRESULT hr = S_OK;

	// 셰이더 코드의 엔트리 포인트를 나중에 유동적으로 받을 수 있게 바꾸어 준다..~
	D3DDevice::GetInstance()->VertexShaderLoad(fxname, "VS_Main", "vs_5_0",&m_pVS, &m_pVSCode);

	D3DDevice::GetInstance()->PixelShaderLoad(fxname, "PS_Main", "ps_5_0", &m_pPS);

	return hr;

}

HRESULT Shader::CreateVBLayOut(D3D11_INPUT_ELEMENT_DESC layout[], UINT layoutsize, ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;

	UINT numElements = layoutsize;

	hr = pDevice->CreateInputLayout(layout,
		numElements,
		m_pVSCode->GetBufferPointer(),
		m_pVSCode->GetBufferSize(),
		&m_pVBLayout
	);
	if (FAILED(hr))	return hr;

	return hr;
}

HRESULT Shader::CreateDynamicConstantBuffer(UINT size, LPVOID pdata, ID3D11Buffer** ppConstantbuff, ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* pCB = nullptr;


	//상수 버퍼 생성전, 크기 테스트.
	//상수버퍼는 16바이트 정렬된 크기로 생성해야 합니다.
	if (size % 16)
	{
		MessageBox(NULL, L"상수버퍼  : 16바이트 미정렬... 버퍼 생성 실패", L"_Error", MB_OK | MB_ICONERROR);
	}
	else
	{

	}


	//상수 버퍼 정보 설정.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;				//동적 정점버퍼 설정.
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//CPU 접근 설정.

	//서브리소스 설정.
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = pdata;				//(외부) 상수 데이터 설정.
	//sd.SysMemPitch = 0;
	//sd.SysMemSlicePitch = 0;

	//상수 버퍼 생성.
	hr = pDevice->CreateBuffer(&bd, &sd, &pCB);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"동적 상수 버퍼 생성 실패", L"_Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	//외부로 전달.
	*ppConstantbuff = pCB;

	return hr;
}

HRESULT Shader::UpdateDynamicConstantBuffer(ID3D11DeviceContext* pDXDC, ID3D11Resource* pBuff, LPVOID pData, UINT size)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	hr = pDXDC->Map(pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
	if (FAILED(hr))
	{
		return hr;
	}

	memcpy(mr.pData, pData, size);

	pDXDC->Unmap(pBuff, 0);


	return hr;
}
void Shader::ShaderRelease()
{
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pVSCode);
	SAFE_RELEASE(m_pVBLayout);
}


