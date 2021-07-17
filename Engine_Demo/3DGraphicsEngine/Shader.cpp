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
	// �ϴ� �⺻ �帧�� �̷��� ���󰣴�...
	HRESULT hr = S_OK;

	// ���̴� �ڵ��� ��Ʈ�� ����Ʈ�� ���߿� ���������� ���� �� �ְ� �ٲپ� �ش�..~
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


	//��� ���� ������, ũ�� �׽�Ʈ.
	//������۴� 16����Ʈ ���ĵ� ũ��� �����ؾ� �մϴ�.
	if (size % 16)
	{
		MessageBox(NULL, L"�������  : 16����Ʈ ������... ���� ���� ����", L"_Error", MB_OK | MB_ICONERROR);
	}
	else
	{

	}


	//��� ���� ���� ����.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;				//���� �������� ����.
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//CPU ���� ����.

	//���긮�ҽ� ����.
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = pdata;				//(�ܺ�) ��� ������ ����.
	//sd.SysMemPitch = 0;
	//sd.SysMemSlicePitch = 0;

	//��� ���� ����.
	hr = pDevice->CreateBuffer(&bd, &sd, &pCB);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"���� ��� ���� ���� ����", L"_Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	//�ܺη� ����.
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


