#include "RenderTargetMgr.h"

RenderTargetMgr::RenderTargetMgr()
	:m_pRtResult(nullptr), m_RtDebugDraw(false), m_Size(), m_DepthSize(), m_BloomOnOff(false)
{
	// 장치 정보 받아오기
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();
}

RenderTargetMgr::~RenderTargetMgr()
{
	if (m_pUI_Depth)
	{
		delete m_pUI_Depth;
		m_pUI_Depth = nullptr;
	}

	if (m_pRt_Depth)
	{
		delete m_pRt_Depth;
		m_pRt_Depth = nullptr;
	}
}

void RenderTargetMgr::InitRenderTargetMgr()
{
	// 화면 UI 전용 행렬선언.
	m_View = XMMatrixIdentity();
	m_Proj = XMMatrixOrthographicOffCenterLH(0, (float)D3DDevice::GetInstance()->m_ScreenWidth,
		(float)D3DDevice::GetInstance()->m_ScreenHeight, 0.0f, 1.0f, 100.f);

	m_Size = { 256, 256 };
	/// Shadow
	DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT dsfmt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		//원본 DS 포멧 유지.
	
	/// Depth
	m_pRt_Depth = new RenderTarget();
	fmt = DXGI_FORMAT_R32_FLOAT;
	m_Size = { 4096	, 4096};
	m_pRt_Depth->CreateRenderTarget(m_Size, fmt, dsfmt);

	/// Result
	m_pRtResult = new RenderTarget();
	m_ResultSize = { (int)D3DDevice::GetInstance()->m_ScreenWidth, (int)D3DDevice::GetInstance()->m_ScreenHeight };
	fmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dsfmt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		//원본 DS 포멧 유지.
	m_pRtResult->CreateRenderTarget(m_ResultSize, fmt, dsfmt);

	/// Bloom
	m_pRtBloomCurve = new RenderTarget();
	m_BloomSize = { (int)D3DDevice::GetInstance()->m_ScreenWidth, (int)D3DDevice::GetInstance()->m_ScreenHeight };
	fmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dsfmt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		//원본 DS 포멧 유지.
	m_pRtBloomCurve->CreateRenderTarget(m_BloomSize, fmt, dsfmt);

	/// Bloom_DownSampling_ver1
	m_pRtBloom_DownSmp_1 = new RenderTarget();
	m_BloomSize_smp1 = { (int)D3DDevice::GetInstance()->m_ScreenWidth / 4, (int)D3DDevice::GetInstance()->m_ScreenHeight / 4 };
	fmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dsfmt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		//원본 DS 포멧 유지.
	m_pRtBloom_DownSmp_1->CreateRenderTarget(m_BloomSize_smp1, fmt, dsfmt);

	/// Bloom_DownSampling_ver2
	m_pRtBloom_DownSmp_2 = new RenderTarget();
	m_BloomSize_smp2 = { (int)D3DDevice::GetInstance()->m_ScreenWidth / 8, (int)D3DDevice::GetInstance()->m_ScreenHeight / 8 };
	fmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dsfmt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		//원본 DS 포멧 유지.
	m_pRtBloom_DownSmp_2->CreateRenderTarget(m_BloomSize_smp2, fmt, dsfmt);

	/// Bloom_DownSampling_ver3
	m_pRtBloom_DownSmp_3 = new RenderTarget();
	m_BloomSize_smp3 = { (int)D3DDevice::GetInstance()->m_ScreenWidth / 16, (int)D3DDevice::GetInstance()->m_ScreenHeight / 16 };
	fmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dsfmt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		//원본 DS 포멧 유지.
	m_pRtBloom_DownSmp_3->CreateRenderTarget(m_BloomSize_smp3, fmt, dsfmt);

	/// Bloom_Result
	m_pRtBloom_Result = new RenderTarget();
	fmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dsfmt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		//원본 DS 포멧 유지.
	m_pRtBloom_Result->CreateRenderTarget(m_BloomSize, fmt, dsfmt);
	/// Bloom_Result_2
	m_pRtBloom_Result_2 = new RenderTarget();
	fmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dsfmt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		//원본 DS 포멧 유지.
	m_pRtBloom_Result_2->CreateRenderTarget(m_BloomSize, fmt, dsfmt);
	/// Bloom_Result_3
	m_pRtBloom_Result_3 = new RenderTarget();
	fmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dsfmt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		//원본 DS 포멧 유지.
	m_pRtBloom_Result_3->CreateRenderTarget(m_BloomSize, fmt, dsfmt);
	/// Bloom_Result_4
	m_pRtBloom_Result_4 = new RenderTarget();
	fmt = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dsfmt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;		//원본 DS 포멧 유지.
	m_pRtBloom_Result_4->CreateRenderTarget(m_BloomSize, fmt, dsfmt);


	// Depth Buffer
	m_pUI_Result = new UI(Vector3(0, 0, 1), Vector3(1, 1, 0), m_pRt_Depth->m_pRtShaderTex);
	m_pUI_Bloom = new UI(Vector3(0, 0, 1), Vector3(1, 1, 0), m_pRt_Depth->m_pRtShaderTex);
	m_pUI_Bloom_DownSmp_1 = new UI(Vector3(256, 0, 1), Vector3(1, 1, 0), m_pRt_Depth->m_pRtShaderTex);
	m_pUI_Bloom_DownSmp_2 = new UI(Vector3(512, 0, 1), Vector3(1, 1, 0), m_pRt_Depth->m_pRtShaderTex);
	m_pUI_Bloom_DownSmp_3 = new UI(Vector3(768, 0, 1), Vector3(1, 1, 0), m_pRt_Depth->m_pRtShaderTex);
	m_pUI_Depth = new UI(Vector3(1024, 0, 1), Vector3(1, 1, 0), m_pRt_Depth->m_pRtShaderTex);

}

Matrix RenderTargetMgr::GetLightVPMatrix(Vector3 targetPos, float radious)
{

	m_Light = D3DDevice::GetInstance()->GiveDirectionLightData();

	Vector3 Litdir = Vector3(m_Light.direction.x, m_Light.direction.y, m_Light.direction.z);
	Litdir.Normalize();
	Vector3 upvec = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 posvec = Litdir * radious * -2.0f;
	Vector3 lookvec = targetPos; // Origin Pos -> Target

	Matrix View = XMMatrixLookAtLH(posvec, lookvec, upvec);

	Vector3 origin = XMVector3TransformCoord(lookvec, View);

	float l = origin.x - radious;
	float r = origin.x + radious;
	float b = origin.y - radious;
	float t = origin.y + radious;
	float n = origin.z - radious;
	float f = origin.z + radious;

	Matrix Proj = XMMatrixOrthographicLH(r - l, t - b, n, f);
	//Matrix Proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60), D3DDevice::GetInstance()->m_ScreenWidth / D3DDevice::GetInstance()->m_ScreenHeight, 1, 300);

	return View * Proj;
}

void RenderTargetMgr::DrawRt_Debug()
{
	if (m_BloomOnOff == true)
	{
		m_pUI_Result->DrawUI_RT_FINAL(m_pRtResult->m_pRtShaderTex, m_pRtBloom_Result_3->m_pRtShaderTex, D3DDevice::GetInstance()->m_ScreenWidth, D3DDevice::GetInstance()->m_ScreenHeight);

		/// _1 - 블룸커브 추출
		SetViewPort(m_BloomSize.cx, m_BloomSize.cy);
		m_pRtBloomCurve->UpdataRt(Vector4(1, 1, 1, 1));
		m_pUI_Bloom->DrawUI_RT_Bloom(m_pRtResult->m_pRtShaderTex, m_BloomSize.cx, m_BloomSize.cy);
		SetViewPort(D3DDevice::GetInstance()->m_ScreenWidth, D3DDevice::GetInstance()->m_ScreenHeight);
		m_pRtBloomCurve->RestoreRt();

		/// _2 DownSample
		SetViewPort(m_BloomSize_smp1.cx, m_BloomSize_smp1.cy);
		m_pRtBloom_DownSmp_1->UpdataRt(Vector4(1, 1, 1, 1));
		m_pUI_Bloom->DrawUI_RT(m_pRtBloomCurve->m_pRtShaderTex, m_BloomSize.cx, m_BloomSize.cy);
		SetViewPort(D3DDevice::GetInstance()->m_ScreenWidth, D3DDevice::GetInstance()->m_ScreenHeight);
		m_pRtBloom_DownSmp_1->RestoreRt();

		/// _3 DownSample
		SetViewPort(m_BloomSize_smp2.cx, m_BloomSize_smp2.cy);
		m_pRtBloom_DownSmp_2->UpdataRt(Vector4(1, 1, 1, 1));
		m_pUI_Bloom->DrawUI_RT(m_pRtBloom_DownSmp_1->m_pRtShaderTex, m_BloomSize.cx, m_BloomSize.cy);
		SetViewPort(D3DDevice::GetInstance()->m_ScreenWidth, D3DDevice::GetInstance()->m_ScreenHeight);
		m_pRtBloom_DownSmp_2->RestoreRt();

		/// _4 DownSample
		SetViewPort(m_BloomSize_smp3.cx, m_BloomSize_smp3.cy);
		m_pRtBloom_DownSmp_3->UpdataRt(Vector4(1, 1, 1, 1));
		m_pUI_Bloom->DrawUI_RT(m_pRtBloom_DownSmp_2->m_pRtShaderTex, m_BloomSize.cx, m_BloomSize.cy);
		SetViewPort(D3DDevice::GetInstance()->m_ScreenWidth, D3DDevice::GetInstance()->m_ScreenHeight);
		m_pRtBloom_DownSmp_3->RestoreRt();

		/// _5 UpSampling
		SetViewPort(m_BloomSize.cx, m_BloomSize.cy);
		m_pRtBloom_Result->UpdataRt(Vector4(1, 1, 1, 1));
		m_pUI_Bloom->DrawUI_RT_Blur_ACC(m_pRtBloom_DownSmp_3->m_pRtShaderTex, m_pRtBloom_DownSmp_2->m_pRtShaderTex, m_BloomSize.cx, m_BloomSize.cy);
		SetViewPort(D3DDevice::GetInstance()->m_ScreenWidth, D3DDevice::GetInstance()->m_ScreenHeight);
		m_pRtBloom_Result->RestoreRt();

		/// _6 UpSampling
		SetViewPort(m_BloomSize.cx, m_BloomSize.cy);
		m_pRtBloom_Result_2->UpdataRt(Vector4(1, 1, 1, 1));
		m_pUI_Bloom->DrawUI_RT_Blur(m_pRtBloom_DownSmp_1->m_pRtShaderTex, m_BloomSize.cx, m_BloomSize.cy, 0);
		SetViewPort(D3DDevice::GetInstance()->m_ScreenWidth, D3DDevice::GetInstance()->m_ScreenHeight);
		m_pRtBloom_Result_2->RestoreRt();

		/// _7 UpSampling
		SetViewPort(m_BloomSize.cx, m_BloomSize.cy);
		m_pRtBloom_Result_3->UpdataRt(Vector4(1, 1, 1, 1));
		m_pUI_Bloom->DrawUI_RT_FINAL(m_pRtBloom_Result->m_pRtShaderTex, m_pRtBloom_Result_2->m_pRtShaderTex, m_BloomSize.cx, m_BloomSize.cy);
		SetViewPort(D3DDevice::GetInstance()->m_ScreenWidth, D3DDevice::GetInstance()->m_ScreenHeight);
		m_pRtBloom_Result_3->RestoreRt();

		///// _7 UpSampling
		//SetViewPort(m_BloomSize.cx, m_BloomSize.cy);
		//m_pRtBloom_Result_4->UpdataRt(Vector4(1, 1, 1, 1));
		//m_pUI_Bloom->DrawUI_RT_Blur_ACC(m_pRtBloom_Result_3->m_pRtShaderTex, m_pRtBloomCurve->m_pRtShaderTex, m_BloomSize.cx, m_BloomSize.cy);
		//SetViewPort(D3DDevice::GetInstance()->m_ScreenWidth, D3DDevice::GetInstance()->m_ScreenHeight);
		//m_pRtBloom_Result_4->RestoreRt();

	}
	else
	{
		//m_pUI_Result->DrawUI_RT(m_pRtResult->m_pRtShaderTex, D3DDevice::GetInstance()->m_ScreenWidth, D3DDevice::GetInstance()->m_ScreenHeight);
	}

	if (GetAsyncKeyState(VK_F9) & 0x8001 == 0x8001)
	{
		m_RtDebugDraw ^= true;
	}

	if (m_RtDebugDraw == true)
	{
		m_pUI_Bloom->DrawUI_RT(m_pRtBloomCurve->m_pRtShaderTex, 256, 256);
		m_pUI_Bloom_DownSmp_1->DrawUI_RT(m_pRtBloom_Result->m_pRtShaderTex, 256, 256);
		m_pUI_Bloom_DownSmp_2->DrawUI_RT(m_pRtBloom_Result_2->m_pRtShaderTex, 256, 256);
		m_pUI_Bloom_DownSmp_3->DrawUI_RT(m_pRtBloom_Result_3->m_pRtShaderTex, 256, 256);
		m_pUI_Depth->DrawUI_RT(m_pRt_Depth->m_pRtShaderTex, 256, 256);
	}

}


void RenderTargetMgr::UpdateAllRenderTarget()
{

}

void RenderTargetMgr::RestoreAllRenderTarget()
{

}


void RenderTargetMgr::UpdateRenderTarget_Depth()
{
	SetViewPort(m_Size.cx, m_Size.cy);
	m_pRt_Depth->UpdataRt(Vector4(1, 1, 1, 1));
}
void RenderTargetMgr::RestoreRenderTarget_Depth()
{
	SetViewPort(D3DDevice::GetInstance()->m_ScreenWidth, D3DDevice::GetInstance()->m_ScreenHeight);
	m_pRt_Depth->RestoreRt();
}

void RenderTargetMgr::SetViewPort(UINT w, UINT h)
{
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (FLOAT)w;
	vp.Height = (FLOAT)h;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_pDevConCopy->RSSetViewports(1, &vp);
}

void RenderTargetMgr::UpdateRT_Result()
{
	SetViewPort(m_ResultSize.cx, m_ResultSize.cy);
	m_pRtResult->UpdataRt(Vector4(1, 1, 1, 1));
}

void RenderTargetMgr::ReStoreRT_Result()
{
	SetViewPort(m_ResultSize.cx, m_ResultSize.cy);
	m_pRtResult->RestoreRt();
}

void RenderTargetMgr::UpdateRT_Bloom()
{
	SetViewPort(m_Size.cx, m_Size.cy);
	m_pRtBloomCurve->UpdataRt(Vector4(1, 1, 1, 1));
}

void RenderTargetMgr::ReStoreRT_Bloom()
{
	SetViewPort(m_Size.cx, m_Size.cy);
	m_pRtBloomCurve->RestoreRt();
}

ID3D11ShaderResourceView* RenderTargetMgr::GetDepthTex()
{
	return m_pRt_Depth->m_pRtShaderTex;
}
