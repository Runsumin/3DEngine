#include "RenderTarget.h"

RenderTarget::RenderTarget()
	:m_pRtTex(nullptr), m_pRtView(nullptr), m_pRtShaderTex(nullptr)
	, m_pRtTexBuff(nullptr), m_pRtViewScene(nullptr)
{
	// ��ġ ���� �޾ƿ���
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();
}

RenderTarget::~RenderTarget()
{
	ReleaseRt();
}

void RenderTarget::CreateRenderTarget(SIZE size, DXGI_FORMAT fmt, DXGI_FORMAT dsfmt)
{
	// ����Ÿ�ٿ� ���ҽ� ����...
	//DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
	//DXGI_FORMAT dsFmt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		//���� DS ���� ����.

	CreateRtTexture(size.cx, size.cy, fmt);
	CreateRtView(fmt);
	CreateRtShaderView(fmt);
	CreateRtDepthStencil(size.cx, size.cy, dsfmt);

}

void RenderTarget::UpdataRt(Vector4 col)
{
	// �ϴ� ���� ���� ���� ����
	m_pDevConCopy->OMSetRenderTargets(1, &m_pRtView, m_pRtViewScene);

	//Vector4 col = Vector4(1, 1, 1, 1);	// ����� ����...
	m_pDevConCopy->ClearRenderTargetView(m_pRtView, (float*)&col);
	m_pDevConCopy->ClearDepthStencilView(m_pRtViewScene,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void RenderTarget::RestoreRt()
{
	m_pDevConCopy->OMSetRenderTargets(1, &D3DDevice::GetInstance()->m_RendertargetView,
		D3DDevice::GetInstance()->m_pDSView);
}

HRESULT RenderTarget::CreateRtTexture(UINT width, UINT height, DXGI_FORMAT format)
{
	//�ؽ�ó ���� ����.
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = format;
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	// 
	//�ؽ�ó ����.
	HRESULT hr = m_pDevCopy->CreateTexture2D(&td, NULL, &m_pRtTex);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Rt 2D �ؽ��� �ε� ����", L"_Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	return hr;
}

HRESULT RenderTarget::CreateRtView(DXGI_FORMAT format)
{
	//����Ÿ�� ���� ����.
	D3D11_RENDER_TARGET_VIEW_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.Format = format;										//�ؽ�ó�� ������������.
	rd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rd.Texture2D.MipSlice = 0;
	//rd.Texture2DMS.UnusedField_NothingToDefine = 0;		

	//����Ÿ�� ����.
	HRESULT hr = m_pDevCopy->CreateRenderTargetView(m_pRtTex, &rd, &m_pRtView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Rt View ���� ����", L"_Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	return hr;
}

HRESULT RenderTarget::CreateRtShaderView(DXGI_FORMAT format)
{
	//���̴����ҽ��� ���� ����.
	D3D11_SHADER_RESOURCE_VIEW_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Format = format;										//�ؽ�ó�� ������������.
	sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sd.Texture2D.MipLevels = 1;
	sd.Texture2D.MostDetailedMip = 0;
	//sd.Texture2DMS.UnusedField_NothingToDefine = 0;

	//���̴����ҽ��� ����.
	HRESULT hr = m_pDevCopy->CreateShaderResourceView(m_pRtTex, &sd, &m_pRtShaderTex);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Rt Shader View ���� ����", L"_Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	return hr;
}

HRESULT RenderTarget::CreateRtDepthStencil(UINT width, UINT height, DXGI_FORMAT format)
{
	HRESULT hr = S_OK;

	//����/���ٽ� ���� ���� ����.
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = format;								//���� RT �� ���� ��������.
	td.SampleDesc.Count = 1;						// AA ����.
	//td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;		//����-���ٽ� ���ۿ����� ����.
	td.MiscFlags = 0;
	td.CPUAccessFlags = 0;


	//����/���ٽ� ���ۿ� �� �ؽ�ó�� �����.	
	hr = m_pDevCopy->CreateTexture2D(&td, NULL, &m_pRtTexBuff);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Rt DS Tex ���� ����", L"_Error", MB_OK | MB_ICONERROR);
		//return hr;
	}

	//---------------------------------- 
	// ����/���ٽ� �� ����.
	//---------------------------------- 
	D3D11_DEPTH_STENCIL_VIEW_DESC dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Format = td.Format;
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;	
	dd.Texture2D.MipSlice = 0;
	//����/���ٽ� �� ����.
	hr = m_pDevCopy->CreateDepthStencilView(m_pRtTexBuff, &dd, &m_pRtViewScene);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Rt DS TexView ���� ����", L"_Error", MB_OK | MB_ICONERROR);
		//return hr;
	}

	return hr;
}

void RenderTarget::ReleaseRt()
{
	// �ϴ��� ����Ÿ�� ������...���� ����
	SAFE_RELEASE(m_pRtTex);
	SAFE_RELEASE(m_pRtView);
	SAFE_RELEASE(m_pRtShaderTex);
	SAFE_RELEASE(m_pRtTexBuff);
}
