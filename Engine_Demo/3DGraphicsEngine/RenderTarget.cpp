#include "RenderTarget.h"

RenderTarget::RenderTarget()
	:m_pRtTex(nullptr), m_pRtView(nullptr), m_pRtShaderTex(nullptr)
	, m_pRtTexBuff(nullptr), m_pRtViewScene(nullptr)
{
	// 장치 정보 받아오기
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();
}

RenderTarget::~RenderTarget()
{
	ReleaseRt();
}

void RenderTarget::CreateRenderTarget(SIZE size, DXGI_FORMAT fmt, DXGI_FORMAT dsfmt)
{
	// 렌더타겟에 리소스 생성...
	//DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
	//DXGI_FORMAT dsFmt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		//원본 DS 포멧 유지.

	CreateRtTexture(size.cx, size.cy, fmt);
	CreateRtView(fmt);
	CreateRtShaderView(fmt);
	CreateRtDepthStencil(size.cx, size.cy, dsfmt);

}

void RenderTarget::UpdataRt(Vector4 col)
{
	// 일단 기존 깊이 버퍼 공유
	m_pDevConCopy->OMSetRenderTargets(1, &m_pRtView, m_pRtViewScene);

	//Vector4 col = Vector4(1, 1, 1, 1);	// 백버퍼 색깔...
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
	//텍스처 정보 구성.
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
	//텍스처 생성.
	HRESULT hr = m_pDevCopy->CreateTexture2D(&td, NULL, &m_pRtTex);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Rt 2D 텍스쳐 로드 실패", L"_Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	return hr;
}

HRESULT RenderTarget::CreateRtView(DXGI_FORMAT format)
{
	//렌더타겟 정보 구성.
	D3D11_RENDER_TARGET_VIEW_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.Format = format;										//텍스처와 동일포멧유지.
	rd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rd.Texture2D.MipSlice = 0;
	//rd.Texture2DMS.UnusedField_NothingToDefine = 0;		

	//렌더타겟 생성.
	HRESULT hr = m_pDevCopy->CreateRenderTargetView(m_pRtTex, &rd, &m_pRtView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Rt View 생성 실패", L"_Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	return hr;
}

HRESULT RenderTarget::CreateRtShaderView(DXGI_FORMAT format)
{
	//셰이더리소스뷰 정보 구성.
	D3D11_SHADER_RESOURCE_VIEW_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Format = format;										//텍스처와 동일포멧유지.
	sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sd.Texture2D.MipLevels = 1;
	sd.Texture2D.MostDetailedMip = 0;
	//sd.Texture2DMS.UnusedField_NothingToDefine = 0;

	//셰이더리소스뷰 생성.
	HRESULT hr = m_pDevCopy->CreateShaderResourceView(m_pRtTex, &sd, &m_pRtShaderTex);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Rt Shader View 생성 실패", L"_Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	return hr;
}

HRESULT RenderTarget::CreateRtDepthStencil(UINT width, UINT height, DXGI_FORMAT format)
{
	HRESULT hr = S_OK;

	//깊이/스텐실 버퍼 정보 구성.
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = format;								//원본 RT 와 동일 포멧유지.
	td.SampleDesc.Count = 1;						// AA 없음.
	//td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;		//깊이-스텐실 버퍼용으로 설정.
	td.MiscFlags = 0;
	td.CPUAccessFlags = 0;


	//깊이/스텐실 버퍼용 빈 텍스처로 만들기.	
	hr = m_pDevCopy->CreateTexture2D(&td, NULL, &m_pRtTexBuff);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Rt DS Tex 생성 실패", L"_Error", MB_OK | MB_ICONERROR);
		//return hr;
	}

	//---------------------------------- 
	// 깊이/스텐실 뷰 생성.
	//---------------------------------- 
	D3D11_DEPTH_STENCIL_VIEW_DESC dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Format = td.Format;
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;	
	dd.Texture2D.MipSlice = 0;
	//깊이/스텐실 뷰 생성.
	hr = m_pDevCopy->CreateDepthStencilView(m_pRtTexBuff, &dd, &m_pRtViewScene);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Rt DS TexView 생성 실패", L"_Error", MB_OK | MB_ICONERROR);
		//return hr;
	}

	return hr;
}

void RenderTarget::ReleaseRt()
{
	// 일단은 렌더타겟 삭제시...추후 변경
	SAFE_RELEASE(m_pRtTex);
	SAFE_RELEASE(m_pRtView);
	SAFE_RELEASE(m_pRtShaderTex);
	SAFE_RELEASE(m_pRtTexBuff);
}
