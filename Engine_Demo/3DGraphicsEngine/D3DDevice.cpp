#include "D3DDevice.h"

// 인터페이스 네임스페이스 사용
using namespace GRAPHICENGINE;
D3DDevice* D3DDevice::m_Instance = nullptr;

D3DDevice::D3DDevice()
	:m_SwapChain(nullptr), m_Device(nullptr), m_DeviceContect(nullptr), m_RendertargetView(nullptr)
	, m_pDS(NULL), m_pDSView(NULL)
{
	m_FeatureLevels = D3D_FEATURE_LEVEL_11_1;
	m_AA = 8;
	m_Af = 16;
	m_Vsync = true;
}

D3DDevice::~D3DDevice()
{
	Release();
}

D3DDevice* D3DDevice::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new D3DDevice();
	}

	return m_Instance;
}

bool D3DDevice::EngineSetUp(HWND hwnd)
{
	// 모드 설정
	SetDXGI_Mode(hwnd);

	// 디바이스와 스왑체인 생성
	CreateDeviceSwapChain(hwnd);

	// 렌더타겟 설정
	CreateRenderTarget();

	// 깊이, 스텐실 설정
	CreateDepthStencil();

	// 깊이, 스텐실 상태 설정
	CreateDepthStencilState();

	// 렌더링 상태 설정
	CreateRasterState();

	// 색상 혼합 객체 설정
	CreateBlenderState();

	// 텍스쳐 샘플러 설정
	CreateSamplerState();

	m_DeviceContect->OMSetRenderTargets(1, &m_RendertargetView, m_pDSView);

	SetViewport();

	return true;
}

void D3DDevice::BeginRend(Vector4 backbuffercolor)
{
	ClearBackBufer(D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0, backbuffercolor);
}

void D3DDevice::EndRend()
{
	// 기본 상태로 셋팅 되돌리기...
	m_DeviceContect->OMSetDepthStencilState(m_DSState[DS_DEPTH_ON], 0);
	m_DeviceContect->PSSetSamplers(0, 1, &m_pSampler[SS_DEFAULT]);
	Flip();
}

bool D3DDevice::SetDXGI_Mode(HWND hwnd)
{
	RECT rc;
	GetClientRect(hwnd, &rc);
	m_ScreenWidth = static_cast<float>(rc.right);
	m_ScreenHeight = static_cast<float>(rc.bottom);
	m_Mode = { (UINT)m_ScreenWidth, (UINT)m_ScreenHeight, 0, 1, DXGI_FORMAT_R8G8B8A8_UNORM };

	return true;
}

HRESULT D3DDevice::CreateDeviceSwapChain(HWND hwnd)
{
	HRESULT hs = S_OK;

	UINT creationFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(_DEBUG)
	// If the project is in a debug build, enable the debug layer.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC swapchaindesc;
	ZeroMemory(&swapchaindesc, sizeof(swapchaindesc));
	swapchaindesc.Windowed = TRUE;										// 풀스크린 & 창모드
	swapchaindesc.OutputWindow = hwnd;
	swapchaindesc.BufferCount = 1;
	swapchaindesc.BufferDesc.Width = m_Mode.Width;
	swapchaindesc.BufferDesc.Height = m_Mode.Height;
	swapchaindesc.BufferDesc.Format = m_Mode.Format;
	swapchaindesc.BufferDesc.RefreshRate.Numerator = m_Vsync ? 60 : 0;
	swapchaindesc.BufferDesc.RefreshRate.Denominator = 1;
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// 용도를 렌더타겟으로 성정	 
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchaindesc.SampleDesc.Count = m_AA;
	swapchaindesc.SampleDesc.Quality = 0;
	swapchaindesc.Flags = 0;

	hs = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_SINGLETHREADED,
		&m_FeatureLevels,
		1,
		D3D11_SDK_VERSION,
		&swapchaindesc,
		&m_SwapChain,
		&m_Device,
		NULL,
		&m_DeviceContect);

	// 에러처리 추후 추가

	if (FAILED(hs))
	{
		return FALSE;
	}
	return hs;

}

HRESULT D3DDevice::CreateRenderTarget()
{
	HRESULT hr = S_OK;

	ID3D11Texture2D* pBackBuffer;

	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))	return hr;

	hr = m_Device->CreateRenderTargetView(pBackBuffer, NULL, &m_RendertargetView);
	if (FAILED(hr))
	{
		return hr;
	}
	SAFE_RELEASE(pBackBuffer);

	return hr;
}

void D3DDevice::SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (FLOAT)m_Mode.Width;
	vp.Height = (FLOAT)m_Mode.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_DeviceContect->RSSetViewports(1, &vp);
}

void D3DDevice::SetViewport(float Screenwidth, float screenheight)
{
	m_ScreenWidth = Screenwidth;
	m_ScreenHeight = screenheight;

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = m_ScreenWidth;
	vp.Height = m_ScreenHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_DeviceContect->RSSetViewports(1, &vp);
}

void D3DDevice::Release()
{
	if (m_DeviceContect) m_DeviceContect->ClearState();

	SAFE_RELEASE(m_SwapChain);
	SAFE_RELEASE(m_Device);
	SAFE_RELEASE(m_DeviceContect);
	SAFE_RELEASE(m_RendertargetView);
	SAFE_RELEASE(m_pDS);
	SAFE_RELEASE(m_pDSView);

	for (int i = 0; i < DS_MAX_; i++)
		SAFE_RELEASE(m_DSState[i]);
	for (int i = 0; i < RS_MAX_; i++)
		SAFE_RELEASE(m_RState[i]);
	for (int i = 0; i < BS_MAX_; i++)
		SAFE_RELEASE(m_BState[i]);
	for (int i = 0; i < SS_MAX; i++)
		SAFE_RELEASE(m_pSampler[i]);

}

void D3DDevice::ChangeRenderState(RenderState renderstate)
{
	switch (renderstate)
	{
	case RenderState::CULLBACK:
		m_DeviceContect->RSSetState(m_RState[RS_CULLBACK]);
		break;
	case RenderState::CULLFRONT:
		m_DeviceContect->RSSetState(m_RState[RS_CULLFRONT]);
		break;
	case RenderState::WIREFRAME:
		m_DeviceContect->RSSetState(m_RState[RS_WIREFRM]);
		break;
	case RenderState::SOLID:
		m_DeviceContect->RSSetState(m_RState[RS_SOLID]);
		break;
	default:
		break;
	}
}

void D3DDevice::GetCameraMatrixOnce(Matrix view, Matrix proj)
{
	m_View = view;
	m_Proj = proj;
}

void D3DDevice::GetDirectionLightData(Vector4 Direction, Vector4 Diffuse, Vector4 Ambient)
{
	m_DirectionLight.direction = Direction;
	m_DirectionLight.diffuse = Diffuse;
	m_DirectionLight.ambient = Ambient;
}

CBLIGHT D3DDevice::GiveDirectionLightData()
{
	return m_DirectionLight;
}

void D3DDevice::ClearBackBufer(UINT flag, float depth, UINT stencil, Vector4 col)
{
	m_DeviceContect->ClearRenderTargetView(m_RendertargetView, (float*)&col);
	m_DeviceContect->ClearDepthStencilView(m_pDSView, flag, depth, stencil);
}

void D3DDevice::Flip()
{
	m_SwapChain->Present(0, 0);
}

float D3DDevice::GetEngineTime()
{
	static int oldtime = GetTickCount();
	int nowtime = GetTickCount();
	float dTime = (nowtime - oldtime) * 0.001f;
	oldtime = nowtime;

	return dTime;
}

float D3DDevice::GetFramePerSecond()
{
	static int oldtime = GetTickCount();
	int nowtime = GetTickCount();

	static int frmcnt = 0;
	static float fps = 0.0f;

	++frmcnt;

	// 1프레임 돌때 걸린 시간
	int time = nowtime - oldtime;

	// 1초 지나면
	if (time >= 999)
	{
		oldtime = nowtime;

		fps = (float)frmcnt * 1000 / (float)time;
		frmcnt = 0;
	}


	return fps;
}

void D3DDevice::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC  ds;
	//깊이 버퍼 설정 (기본값)
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS;
	//스텐실 버퍼 설정 (기본값) 
	ds.StencilEnable = FALSE;									//스텐실 버퍼 OFF.
	ds.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;		//스텐실 읽기 마스크 (8bit: 0xff)
	ds.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;		//스텐실 쓰기 마스크 (8bit: 0xff)
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;			//[앞면] 스텐실 비교 함수 : "Always" 즉, 항상 성공 (통과, pass)
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;			//[앞면] 스텐실 비교 성공시 동작 : 기존값 유지.
	ds.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;			//[앞면] 스텐실 비교 실패시 동작 : 기존값 유지.	
	ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	//[앞면] 스텐실/깊이 비교 실패시 동작 : 기존값 유지.
	ds.BackFace = ds.FrontFace;									//[뒷면] 설정 동일. 필요시 개별 설정이 가능.

	//----------------------------------------------------------------------
	// 깊이 버퍼 연산 객체들.
	//----------------------------------------------------------------------
	// DS 상태 객체 #0 : Z-Test ON! (기본값)
	ds.DepthEnable = TRUE;					//깊이 버퍼 On.
	ds.StencilEnable = FALSE;				//스텐실 버퍼 Off!
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON]);

	// DS 상태 객체 #1 : Z-Test OFF 상태.
	ds.DepthEnable = FALSE;
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_OFF]);

	// DS 상태 객체 #2 : Z-Test On + Z-Write OFF.
	// Z-Test (ZEnable, DepthEnable) 이 꺼지면, Z-Write 역시 비활성화 됩니다.
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		//깊이값 쓰기 끔.
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_WRITE_OFF]);


	//----------------------------------------------------------------------
	// 스텐실 버퍼 연산 객체들 생성.
	//----------------------------------------------------------------------
	// 스텐실 버퍼 비트 연산 공식.
	// (Stencil.Ref & Stencil.Mask) Comparison-Func ( StencilBuffer.Value & Stencil.Mask)
	//
	// *StencilBufferValue : 현재 검사할 픽셀의 스텐실값.
	// *ComFunc : 비교 함수. ( > < >= <= ==  Always Never)
	//----------------------------------------------------------------------
	// DS 상태객체 #4 :  깊이버퍼 On, 스텐실버퍼 ON (항상, 참조값 쓰기) : "깊이/스텐실 기록" 
	ds.DepthEnable = TRUE;										//깊이버퍼 ON! (기본값)
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS;
	ds.StencilEnable = TRUE;										//스텐실 버퍼 ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//비교함수 : "항상 통과" (성공)
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//성공시 : 참조값(Stencil Reference Value) 로 교체.
	//ds.FrontFace.StencilFailOp	  = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.(기본값, 생략)
	//ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.(기본값, 생략)
	ds.BackFace = ds.FrontFace;										//뒷면 설정 동일.
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_ON]);


	// DS 상태객체 #5 : 깊이버퍼 On, 스텐실버퍼 ON (동일비교, 성공시 유지) : "지정 위치에만 그리기" 
	//ds.DepthEnable	= TRUE;										//깊이버퍼 ON! (기본값)(생략)
	ds.StencilEnable = TRUE;										//스텐실 버퍼 ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//비교함수 : "동일한가?" 
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//성공시 : 유지.
	ds.BackFace = ds.FrontFace;										//뒷면 설정 동일.
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_EQUAL_KEEP]);


	// DS 상태객체 #6 : 깊이버퍼 On, 스텐실버퍼 ON (다름비교, 성공시 유지) : "지정 위치 이외에 그리기" 
	//ds.DepthEnable	= TRUE;										//깊이버퍼 ON! (기본값)(생략)
	ds.StencilEnable = TRUE;										//스텐실 버퍼 ON!
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;	//비교함수 : "같이 않은가?" 
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//성공시 : 유지.
	ds.BackFace = ds.FrontFace;										//뒷면 설정 동일.
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_NOTEQUAL_KEEP]);


	// DS 상태객체 #9 : 깊이버퍼 On, 스텐실버퍼 ON (항상, 성공시 증가) : "스텐실만 기록" 
	ds.DepthEnable = TRUE;										//깊이버퍼 ON! (기본값)
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				//깊이버퍼 쓰기 OFF.
	ds.DepthFunc = D3D11_COMPARISON_LESS;						//깊이연산 ON. (기본값)
	ds.StencilEnable = TRUE;										//스텐실 버퍼 ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//비교함수 : "항상 통과" (성공)
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//성공시 : 참조값(Stencil Reference Value) 로 교체.
	ds.BackFace = ds.FrontFace;										//뒷면 설정 동일.
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_WRITE_OFF_STENCIL_ON]);


	//깊이버퍼 Off (Write Off), 스텐실버퍼 ON (동일비교, 성공시 증가) : "이중그리기 방지.
	ds.DepthEnable = FALSE;											//깊이테스트 Off!
	//ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				//깊이값 쓰기 Off.
	//ds.DepthFunc		= D3D11_COMPARISON_LESS;
	ds.StencilEnable = TRUE;										//스텐실 버퍼 ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;				//비교함수 : "동일한가?" 
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;				//성공시 : 증가 (+1) 
	ds.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;				//실패시 : 유지.
	ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//실패시 : 유지.
	ds.BackFace = ds.FrontFace;										//뒷면 설정 동일.
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_OFF_STENCIL_EQUAL_INCR]);

}

void D3DDevice::CreateRasterState()
{
	//[상태객체 1] 기본 렌더링 상태 개체.
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;		//삼각형 색상 채우기.(기본값)
	rd.CullMode = D3D11_CULL_NONE;		//컬링 없음. (기본값은 컬링 Back)		
	rd.FrontCounterClockwise = false;   //이하 기본값...
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = true;		//AA 적용.
	rd.AntialiasedLineEnable = false;
	//레스터라이져 상태 객체 생성.
	m_Device->CreateRasterizerState(&rd, &m_RState[RS_SOLID]);


	//[상태객체2] 와이어 프레임 그리기. 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	//레스터라이져 객체 생성.
	m_Device->CreateRasterizerState(&rd, &m_RState[RS_WIREFRM]);

	//[상태객체3] 후면 컬링 On! "CCW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	m_Device->CreateRasterizerState(&rd, &m_RState[RS_CULLBACK]);

	//[상태객체4] 와이어 프레임 + 후면컬링 On! "CCW"
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	m_Device->CreateRasterizerState(&rd, &m_RState[RS_WIRECULLBACK]);

	//[상태객체5] 정면 컬링 On! "CW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_FRONT;
	m_Device->CreateRasterizerState(&rd, &m_RState[RS_CULLFRONT]);

	//[상태객체6] 와이어 프레임 + 정면컬링 On! "CW" 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_FRONT;
	m_Device->CreateRasterizerState(&rd, &m_RState[RS_WIRECULLFRONT]);
}

void D3DDevice::CreateBlenderState()
{
	//------------------------------
	// 혼합객체1 : 색상혼합 없음. "Blending OFF"
	//------------------------------
	//블렌딩 상태 객체 구성 옵션 : 이하 기본값들.
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.RenderTarget[0].BlendEnable = FALSE;							//블렌딩 동작 결정. 기본값은 FALSE(OFF)		
	//색상 성분 혼합 : Color Blending.(기본값)
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//색상 혼합 연산(Color - Operation), 기본값은 덧셈 : 최종색 = Src.Color + Dest.Color 	
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;					//소스(원본) 혼합 비율, 원본 100% : Src.Color = Src * 1;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;				//타겟(대상) 혼합 비율, 대상 0%   : Dest.Color = Dest * 0;  통상 0번 RT 는 "백버퍼"를 의미합니다.	
	//알파 성분 혼합 : Alpha Blending.(기본값)
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;			//알파 혼합 함수(Alpha - Opertion), 기본값은 덧셈.
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;				//소스(원본) 알파 혼합 비율.
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;			//타겟(대상) 알파 혼합 비율.
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	//렌더타겟에 쓰기 옵션.
	//bd.AlphaToCoverageEnable = FALSE;								//이하 추가 옵션.(기본값, 생략)
	//bd.IndependentBlendEnable = FALSE;
	m_Device->CreateBlendState(&bd, &m_BState[BS_DEFAULT]);		//상태 개체.생성.


	//------------------------------
	// 혼합객체2 : 알파 혼합 "Alpha Blending"
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//색상 혼합 ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//색상 혼합 연산 (덧셈, 기본값) : 최종색 = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			//소스(원본) 혼합 비율, 원본 알파비율  : Src.Color = Src * Src.a;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;		//타겟(대상) 혼합 비율, 원본 알파비율 반전 : Dest.Color = Dest * (1-Src.a);			 		
	m_Device->CreateBlendState(&bd, &m_BState[BS_ALPHA_BLEND]);


	//------------------------------
	// 혼합객체3 : 색상 혼합 "Color Blending"
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//색상 혼합 ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//색상 혼합 연산 (덧셈, 기본값) : 최종색 = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;			//소스(원본) 혼합 비율, 원본 색상  : Src.Color = Src * Src.Color;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;		//타겟(대상) 혼합 비율, 원본 색상 반전 : Dest.Color = Dest * (1-Src.Color);			 	
	m_Device->CreateBlendState(&bd, &m_BState[BS_COLOR_BLEND]);


	//------------------------------
	// 혼합객체3 : 색상 혼합 "밝기 강조 Blending 1:1" 
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//색상 혼합 ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//색상 혼합 연산 "밝기강조" : 최종색 = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;					//소스(원본) 혼합 비율, 원본 색상  : Src.Color = Src * 1;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;					//타겟(대상) 혼합 비율, 원본 색상 반전 : Dest.Color = Dest * 1;			 	
	m_Device->CreateBlendState(&bd, &m_BState[BS_COLOR_BLEND_ONE]);
}

void D3DDevice::CreateSamplerState()
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));

	//텍스처 필터 : 비등방 필터링
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = m_Af;
	//밉멥핑	
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	sd.MipLODBias = 0;
	//이하 기본값 처리..
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.BorderColor[0] = 1;
	sd.BorderColor[1] = 1;
	sd.BorderColor[2] = 1;
	sd.BorderColor[3] = 1;

	//샘플러 객체1 생성. (DX 기본값)
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = m_Device->CreateSamplerState(&sd, &m_pSampler[SS_CLAMP]);
	//hr = m_Device->CreateSamplerState();
	//샘플러 객체2 생성.
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = m_Device->CreateSamplerState(&sd, &m_pSampler[SS_WRAP]);

	sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.BorderColor[0] = 0;
	sd.BorderColor[1] = 0;
	sd.BorderColor[2] = 0;
	sd.BorderColor[3] = 1;
	sd.MaxLOD = 0;
	hr = m_Device->CreateSamplerState(&sd, &m_pSampler[SS_BOARDER]);

	sd.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	sd.BorderColor[0] = 1;
	sd.BorderColor[1] = 1;
	sd.BorderColor[2] = 1;
	sd.BorderColor[3] = 1;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	sd.MaxAnisotropy = 1;
	sd.MaxLOD = FLT_MAX;
	hr = m_Device->CreateSamplerState(&sd, &m_pSampler[SS_SHADOW]);

}

void D3DDevice::SetDepthStencilState(ID3D11DepthStencilState* state, int refcnt)
{
	m_DeviceContect->OMSetDepthStencilState(state, refcnt);
}

void D3DDevice::SetRenderingState(ID3D11RasterizerState* state)
{
	m_DeviceContect->RSSetState(state);
}

void D3DDevice::SetBlendingState(ID3D11BlendState* state)
{
	//m_DeviceContect->OMSetBlendState
}

void D3DDevice::SetSamplerState(ID3D11SamplerState* state)
{
	//
}

HRESULT D3DDevice::ShaderCompile(const TCHAR* filename, const char* entrypoint, const char* shadermodel, ID3DBlob** ppcode)
{
	HRESULT hr = S_OK;
	ID3DBlob* pError = nullptr;

	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;			// 열 우선 행렬처리
	//UINT Flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;		// 행 우선 행렬 처리

#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG;							//디버깅 모드시 옵션 추가.
	Flags |= D3DCOMPILE_SKIP_OPTIMIZATION;				//디버깅 모드시 옵션 추가.
#else
	Flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	hr = D3DCompileFromFile(filename,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint,
		shadermodel,
		Flags,
		0,
		ppcode,
		&pError
	);

	if (FAILED(hr))
	{
		// 오류 출력
		TCHAR func[80] = L"";
		::mbstowcs(func, entrypoint, strlen(entrypoint));
		TCHAR sm[20] = L"";
		::mbstowcs(sm, shadermodel, strlen(shadermodel));

		//셰이더 오류 메세지 읽기.
		TCHAR errw[4096] = L"";
		::mbstowcs(errw, (char*)pError->GetBufferPointer(), pError->GetBufferSize());

		TCHAR herr[1024] = L"아래의 오류를 확인하십시오.";

		TCHAR errmsg[1024];
		_stprintf(errmsg, L"%s \nFile=%s  Entry=%s  Target=%s  \n에러코드(0x%08X) : %s \n\n%s",
			L"셰이더 컴파일 실패", filename, func, sm,
			hr, herr, errw);

		OutputDebugString(L"\n");
		OutputDebugString(errmsg);

		MessageBox(NULL, errmsg, L"_Error", MB_OK | MB_ICONERROR);
	}

	SAFE_RELEASE(pError);
	return hr;
}

HRESULT D3DDevice::VertexShaderLoad(const TCHAR* filename, const char* entrypoint, const char* shadermodel, ID3D11VertexShader** pvs, ID3DBlob** pvscode)
{
	HRESULT hr = S_OK;

	ID3D11VertexShader* pVS = nullptr;
	ID3DBlob* pVSCode = nullptr;

	hr = ShaderCompile(filename, entrypoint, shadermodel, &pVSCode);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"정점 셰이더 로드 실패", L"_Error", MB_OK | MB_ICONERROR);
	}

	hr = m_Device->CreateVertexShader(pVSCode->GetBufferPointer(),
		pVSCode->GetBufferSize(),
		nullptr,
		&pVS);

	if (FAILED(hr))
	{
		SAFE_RELEASE(pVSCode);
		return hr;
	}

	*pvs = pVS;
	if (pVSCode) *pvscode = pVSCode;
	return hr;

}

HRESULT D3DDevice::PixelShaderLoad(const TCHAR* filename, const char* entrypoint, const char* shadermodel, ID3D11PixelShader** pps)
{
	HRESULT hr = S_OK;

	ID3D11PixelShader* pPS = nullptr;
	ID3DBlob* pPSCode = nullptr;

	hr = ShaderCompile(filename, entrypoint, shadermodel, &pPSCode);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"픽셀 셰이더 로드 실패", L"_Error", MB_OK | MB_ICONERROR);
		return hr;
	}

	hr = m_Device->CreatePixelShader(pPSCode->GetBufferPointer(),
		pPSCode->GetBufferSize(),
		nullptr,
		&pPS
	);

	SAFE_RELEASE(pPSCode);
	if (FAILED(hr))	return hr;

	*pps = pPS;

	return hr;
}


TCHAR* D3DDevice::ConvertStringToTCHAR(string& s)
{
	string tstr;
	const char* all = s.c_str();
	int len = 1 + strlen(all);
	wchar_t* t = new wchar_t[len];
	if (NULL == t) throw std::bad_alloc();
	mbstowcs(t, all, len);
	return (TCHAR*)t;

}

std::string D3DDevice::ConvertTCharToString(const TCHAR* ptsz)
{
	int len = wcslen((wchar_t*)ptsz);
	char* psz = new char[2 * len + 1];
	wcstombs(psz, (wchar_t*)ptsz, 2 * len + 1);
	std::string s = psz;
	delete[] psz;
	return s;
}

HRESULT D3DDevice::CreateDepthStencil()
{
	HRESULT hr = S_OK;

	//깊이/스텐실 버퍼용 정보 구성.
	D3D11_TEXTURE2D_DESC   td;
	ZeroMemory(&td, sizeof(td));
	td.Width = m_Mode.Width;
	td.Height = m_Mode.Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	//td.Format = DXGI_FORMAT_D32_FLOAT;			// 32BIT. 깊이 버퍼.
	//td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 깊이 버퍼 (24bit) + 스텐실 (8bit) / 구형 하드웨어 (DX9)
	//td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// 깊이 버퍼 (32bit) + 스텐실 (8bit) / 신형 하드웨어 (DX11)
	td.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;	// 깊이 버퍼 (32bit) + 스텐실 (8bit) / 신형 하드웨어 (DX11)
	td.SampleDesc.Count = m_AA;						// AA 설정 - RT 과 동일 규격 준수.
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;		// 깊이-스텐실 버퍼용으로 설정.
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	// 깊이 버퍼 생성.
	//ID3D11Texture2D* pDS = NULL;						 
	hr = m_Device->CreateTexture2D(&td, NULL, &m_pDS);
	if (FAILED(hr))
	{
		return hr;
	}


	// 깊이-스텐실버퍼용 리소스 뷰 정보 설정. 
	D3D11_DEPTH_STENCIL_VIEW_DESC  dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Format = td.Format;
	//dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; //AA 없음.
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;	//+AA 설정 "MSAA"
	dd.Texture2D.MipSlice = 1;

	//깊이-스텐실 버퍼 뷰 생성.
	hr = m_Device->CreateDepthStencilView(m_pDS, &dd, &m_pDSView);
	if (FAILED(hr))
	{
		return hr;
	}

	//리소스 뷰 생성 후, 불필요한 DX 핸들은 해제해야 합니다.(메모리 누수 방지)
	//SAFE_RELEASE(pDS);

	return hr;
}

HRESULT D3DDevice::LoadTexture(string filename, ID3D11ShaderResourceView** ppTexRV)
{
	HRESULT hr = S_OK;

	ID3D11ShaderResourceView* pTexRV = nullptr;

	TCHAR* finalpath = ConvertStringToTCHAR(filename);
	//TCHAR* fileName = L"../data/sonim.jpg";

	//일반 텍스처 로드.
	//hr = DirectX::CreateWICTextureFromFile( g_pDevice, fileName, nullptr, &g_pTextureRV );	

	//밉멥+텍스처 로드.
	hr = DirectX::CreateWICTextureFromFileEx(m_Device, m_DeviceContect, finalpath, 0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0, D3D11_RESOURCE_MISC_GENERATE_MIPS, WIC_LOADER_DEFAULT,
		nullptr, &pTexRV);

	if (FAILED(hr))
	{
		//DDS 파일로 로드 시도. : +밉멥
	/*	hr = DirectX::CreateDDSTextureFromFileEx( g_pDevice, g_pDXDC, filename, 0,
				D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, //| D3D11_BIND_RENDER_TARGET,
				0,	D3D11_RESOURCE_MISC_GENERATE_MIPS,	false, //_LOADER_DEFAULT,
				nullptr, &pTexRV );	 */
		hr = DirectX::CreateDDSTextureFromFile(m_Device, finalpath, nullptr, &pTexRV);
		if (FAILED(hr))
		{
			return hr;
		}
	}

	if (finalpath)
	{
		delete finalpath;
		finalpath = nullptr;
	}

	//텍스처 밉멥생성.
	//g_pDXDC->GenerateMips(g_pTextureRV);

	//외부로 리턴.
	*ppTexRV = pTexRV;

	return hr;
}

HRESULT D3DDevice::LoadCubeTexture(string filename, ID3D11ShaderResourceView** ppTexRV)
{
	HRESULT hr = S_OK;
	ID3D11ShaderResourceView* pTexRV = nullptr;


	TCHAR* finalpath = ConvertStringToTCHAR(filename);

	hr = DirectX::CreateDDSTextureFromFileEx(m_Device, m_DeviceContect, finalpath, 0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0,
		D3D11_RESOURCE_MISC_TEXTURECUBE,		// 큐브맵 리소스 구성 옵션
		false, nullptr, &pTexRV);
	if (FAILED(hr))
	{
		return hr;
	}

	// 외부로 리턴
	*ppTexRV = pTexRV;

	if (finalpath)
	{
		delete finalpath;
		finalpath = nullptr;
	}

	return hr;
}

HRESULT D3DDevice::CreateVB(void* pBuff, UINT size, ID3D11Buffer** ppVB)
{
	ID3D11Buffer* pvb = nullptr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 용도 : 버텍스 버퍼
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;

	HRESULT hr = m_Device->CreateBuffer(&bd, &rd, &pvb);
	if (FAILED(hr))
	{
		return hr;
	}

	*ppVB = pvb;

	return hr;
}

HRESULT D3DDevice::CreateIB(void* pBuff, UINT size, ID3D11Buffer** ppIB)
{
	ID3D11Buffer* pib = nullptr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;	// 용도 : 인덱스 버퍼
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;
	rd.SysMemPitch = 0;
	rd.SysMemSlicePitch = 0;

	HRESULT hr = m_Device->CreateBuffer(&bd, &rd, &pib);
	if (FAILED(hr))
	{
		return hr;
	}

	*ppIB = pib;

	return hr;
}

HRESULT D3DDevice::CreateVB_Dynamic(void* pBuff, UINT size, ID3D11Buffer** ppVB)
{
	ID3D11Buffer* pvb = nullptr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 용도 : 버텍스 버퍼
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;

	HRESULT hr = m_Device->CreateBuffer(&bd, &rd, &pvb);
	if (FAILED(hr))
	{
		return hr;
	}

	*ppVB = pvb;

	return hr;
}

HRESULT D3DDevice::CreateIB_Dynamic(void* pBuff, UINT size, ID3D11Buffer** ppIB)
{
	ID3D11Buffer* pib = nullptr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;	// 용도 : 인덱스 버퍼
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;
	rd.SysMemPitch = 0;
	rd.SysMemSlicePitch = 0;

	HRESULT hr = m_Device->CreateBuffer(&bd, &rd, &pib);
	if (FAILED(hr))
	{
		return hr;
	}

	*ppIB = pib;

	return hr;
}

HRESULT D3DDevice::UpdateDynamicBuffer(ID3D11Resource* pBuff, LPVOID pData, UINT size)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = m_DeviceContect->Map(pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
	if (FAILED(hr))
	{
		return hr;
	}

	memcpy(mr.pData, pData, size);

	m_DeviceContect->Unmap(pBuff, 0);


	return hr;
}

HRESULT D3DDevice::GetTextureDesc(ID3D11ShaderResourceView* pRV, D3D11_TEXTURE2D_DESC* pDesc)
{
	HRESULT res = S_OK;

	//셰이더 리소스 뷰 정보 획득.
	D3D11_SHADER_RESOURCE_VIEW_DESC dc;
	pRV->GetDesc(&dc);

	//텍스처 정보 획득.
	ID3D11Resource* pRes = nullptr;
	pRV->GetResource(&pRes);
	ID3D11Texture2D* pTex = nullptr;
	pRes->QueryInterface<ID3D11Texture2D>(&pTex);
	if (pTex)
	{
		pTex->GetDesc(pDesc);

		//pTex->Release();
	}

	//SAFE_RELEASE(pRes);

	return res;
}