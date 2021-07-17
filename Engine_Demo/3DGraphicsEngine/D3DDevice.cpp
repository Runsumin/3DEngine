#include "D3DDevice.h"

// �������̽� ���ӽ����̽� ���
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
	// ��� ����
	SetDXGI_Mode(hwnd);

	// ����̽��� ����ü�� ����
	CreateDeviceSwapChain(hwnd);

	// ����Ÿ�� ����
	CreateRenderTarget();

	// ����, ���ٽ� ����
	CreateDepthStencil();

	// ����, ���ٽ� ���� ����
	CreateDepthStencilState();

	// ������ ���� ����
	CreateRasterState();

	// ���� ȥ�� ��ü ����
	CreateBlenderState();

	// �ؽ��� ���÷� ����
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
	// �⺻ ���·� ���� �ǵ�����...
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
	swapchaindesc.Windowed = TRUE;										// Ǯ��ũ�� & â���
	swapchaindesc.OutputWindow = hwnd;
	swapchaindesc.BufferCount = 1;
	swapchaindesc.BufferDesc.Width = m_Mode.Width;
	swapchaindesc.BufferDesc.Height = m_Mode.Height;
	swapchaindesc.BufferDesc.Format = m_Mode.Format;
	swapchaindesc.BufferDesc.RefreshRate.Numerator = m_Vsync ? 60 : 0;
	swapchaindesc.BufferDesc.RefreshRate.Denominator = 1;
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// �뵵�� ����Ÿ������ ����	 
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

	// ����ó�� ���� �߰�

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

	// 1������ ���� �ɸ� �ð�
	int time = nowtime - oldtime;

	// 1�� ������
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
	//���� ���� ���� (�⺻��)
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS;
	//���ٽ� ���� ���� (�⺻��) 
	ds.StencilEnable = FALSE;									//���ٽ� ���� OFF.
	ds.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;		//���ٽ� �б� ����ũ (8bit: 0xff)
	ds.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;		//���ٽ� ���� ����ũ (8bit: 0xff)
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;			//[�ո�] ���ٽ� �� �Լ� : "Always" ��, �׻� ���� (���, pass)
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;			//[�ո�] ���ٽ� �� ������ ���� : ������ ����.
	ds.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;			//[�ո�] ���ٽ� �� ���н� ���� : ������ ����.	
	ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	//[�ո�] ���ٽ�/���� �� ���н� ���� : ������ ����.
	ds.BackFace = ds.FrontFace;									//[�޸�] ���� ����. �ʿ�� ���� ������ ����.

	//----------------------------------------------------------------------
	// ���� ���� ���� ��ü��.
	//----------------------------------------------------------------------
	// DS ���� ��ü #0 : Z-Test ON! (�⺻��)
	ds.DepthEnable = TRUE;					//���� ���� On.
	ds.StencilEnable = FALSE;				//���ٽ� ���� Off!
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON]);

	// DS ���� ��ü #1 : Z-Test OFF ����.
	ds.DepthEnable = FALSE;
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_OFF]);

	// DS ���� ��ü #2 : Z-Test On + Z-Write OFF.
	// Z-Test (ZEnable, DepthEnable) �� ������, Z-Write ���� ��Ȱ��ȭ �˴ϴ�.
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		//���̰� ���� ��.
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_WRITE_OFF]);


	//----------------------------------------------------------------------
	// ���ٽ� ���� ���� ��ü�� ����.
	//----------------------------------------------------------------------
	// ���ٽ� ���� ��Ʈ ���� ����.
	// (Stencil.Ref & Stencil.Mask) Comparison-Func ( StencilBuffer.Value & Stencil.Mask)
	//
	// *StencilBufferValue : ���� �˻��� �ȼ��� ���ٽǰ�.
	// *ComFunc : �� �Լ�. ( > < >= <= ==  Always Never)
	//----------------------------------------------------------------------
	// DS ���°�ü #4 :  ���̹��� On, ���ٽǹ��� ON (�׻�, ������ ����) : "����/���ٽ� ���" 
	ds.DepthEnable = TRUE;										//���̹��� ON! (�⺻��)
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS;
	ds.StencilEnable = TRUE;										//���ٽ� ���� ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//���Լ� : "�׻� ���" (����)
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//������ : ������(Stencil Reference Value) �� ��ü.
	//ds.FrontFace.StencilFailOp	  = D3D11_STENCIL_OP_KEEP;		//���н� : ����.(�⺻��, ����)
	//ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//���н� : ����.(�⺻��, ����)
	ds.BackFace = ds.FrontFace;										//�޸� ���� ����.
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_ON]);


	// DS ���°�ü #5 : ���̹��� On, ���ٽǹ��� ON (���Ϻ�, ������ ����) : "���� ��ġ���� �׸���" 
	//ds.DepthEnable	= TRUE;										//���̹��� ON! (�⺻��)(����)
	ds.StencilEnable = TRUE;										//���ٽ� ���� ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		//���Լ� : "�����Ѱ�?" 
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//������ : ����.
	ds.BackFace = ds.FrontFace;										//�޸� ���� ����.
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_EQUAL_KEEP]);


	// DS ���°�ü #6 : ���̹��� On, ���ٽǹ��� ON (�ٸ���, ������ ����) : "���� ��ġ �̿ܿ� �׸���" 
	//ds.DepthEnable	= TRUE;										//���̹��� ON! (�⺻��)(����)
	ds.StencilEnable = TRUE;										//���ٽ� ���� ON!
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;	//���Լ� : "���� ������?" 
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		//������ : ����.
	ds.BackFace = ds.FrontFace;										//�޸� ���� ����.
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_ON_STENCIL_NOTEQUAL_KEEP]);


	// DS ���°�ü #9 : ���̹��� On, ���ٽǹ��� ON (�׻�, ������ ����) : "���ٽǸ� ���" 
	ds.DepthEnable = TRUE;										//���̹��� ON! (�⺻��)
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				//���̹��� ���� OFF.
	ds.DepthFunc = D3D11_COMPARISON_LESS;						//���̿��� ON. (�⺻��)
	ds.StencilEnable = TRUE;										//���ٽ� ���� ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		//���Լ� : "�׻� ���" (����)
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		//������ : ������(Stencil Reference Value) �� ��ü.
	ds.BackFace = ds.FrontFace;										//�޸� ���� ����.
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_WRITE_OFF_STENCIL_ON]);


	//���̹��� Off (Write Off), ���ٽǹ��� ON (���Ϻ�, ������ ����) : "���߱׸��� ����.
	ds.DepthEnable = FALSE;											//�����׽�Ʈ Off!
	//ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				//���̰� ���� Off.
	//ds.DepthFunc		= D3D11_COMPARISON_LESS;
	ds.StencilEnable = TRUE;										//���ٽ� ���� ON! 
	ds.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;				//���Լ� : "�����Ѱ�?" 
	ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;				//������ : ���� (+1) 
	ds.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;				//���н� : ����.
	ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		//���н� : ����.
	ds.BackFace = ds.FrontFace;										//�޸� ���� ����.
	m_Device->CreateDepthStencilState(&ds, &m_DSState[DS_DEPTH_OFF_STENCIL_EQUAL_INCR]);

}

void D3DDevice::CreateRasterState()
{
	//[���°�ü 1] �⺻ ������ ���� ��ü.
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;		//�ﰢ�� ���� ä���.(�⺻��)
	rd.CullMode = D3D11_CULL_NONE;		//�ø� ����. (�⺻���� �ø� Back)		
	rd.FrontCounterClockwise = false;   //���� �⺻��...
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = true;		//AA ����.
	rd.AntialiasedLineEnable = false;
	//�����Ͷ����� ���� ��ü ����.
	m_Device->CreateRasterizerState(&rd, &m_RState[RS_SOLID]);


	//[���°�ü2] ���̾� ������ �׸���. 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	//�����Ͷ����� ��ü ����.
	m_Device->CreateRasterizerState(&rd, &m_RState[RS_WIREFRM]);

	//[���°�ü3] �ĸ� �ø� On! "CCW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	m_Device->CreateRasterizerState(&rd, &m_RState[RS_CULLBACK]);

	//[���°�ü4] ���̾� ������ + �ĸ��ø� On! "CCW"
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	m_Device->CreateRasterizerState(&rd, &m_RState[RS_WIRECULLBACK]);

	//[���°�ü5] ���� �ø� On! "CW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_FRONT;
	m_Device->CreateRasterizerState(&rd, &m_RState[RS_CULLFRONT]);

	//[���°�ü6] ���̾� ������ + �����ø� On! "CW" 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_FRONT;
	m_Device->CreateRasterizerState(&rd, &m_RState[RS_WIRECULLFRONT]);
}

void D3DDevice::CreateBlenderState()
{
	//------------------------------
	// ȥ�հ�ü1 : ����ȥ�� ����. "Blending OFF"
	//------------------------------
	//���� ���� ��ü ���� �ɼ� : ���� �⺻����.
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.RenderTarget[0].BlendEnable = FALSE;							//���� ���� ����. �⺻���� FALSE(OFF)		
	//���� ���� ȥ�� : Color Blending.(�⺻��)
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//���� ȥ�� ����(Color - Operation), �⺻���� ���� : ������ = Src.Color + Dest.Color 	
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;					//�ҽ�(����) ȥ�� ����, ���� 100% : Src.Color = Src * 1;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;				//Ÿ��(���) ȥ�� ����, ��� 0%   : Dest.Color = Dest * 0;  ��� 0�� RT �� "�����"�� �ǹ��մϴ�.	
	//���� ���� ȥ�� : Alpha Blending.(�⺻��)
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;			//���� ȥ�� �Լ�(Alpha - Opertion), �⺻���� ����.
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;				//�ҽ�(����) ���� ȥ�� ����.
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;			//Ÿ��(���) ���� ȥ�� ����.
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	//����Ÿ�ٿ� ���� �ɼ�.
	//bd.AlphaToCoverageEnable = FALSE;								//���� �߰� �ɼ�.(�⺻��, ����)
	//bd.IndependentBlendEnable = FALSE;
	m_Device->CreateBlendState(&bd, &m_BState[BS_DEFAULT]);		//���� ��ü.����.


	//------------------------------
	// ȥ�հ�ü2 : ���� ȥ�� "Alpha Blending"
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//���� ȥ�� ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//���� ȥ�� ���� (����, �⺻��) : ������ = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			//�ҽ�(����) ȥ�� ����, ���� ���ĺ���  : Src.Color = Src * Src.a;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;		//Ÿ��(���) ȥ�� ����, ���� ���ĺ��� ���� : Dest.Color = Dest * (1-Src.a);			 		
	m_Device->CreateBlendState(&bd, &m_BState[BS_ALPHA_BLEND]);


	//------------------------------
	// ȥ�հ�ü3 : ���� ȥ�� "Color Blending"
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//���� ȥ�� ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//���� ȥ�� ���� (����, �⺻��) : ������ = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;			//�ҽ�(����) ȥ�� ����, ���� ����  : Src.Color = Src * Src.Color;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;		//Ÿ��(���) ȥ�� ����, ���� ���� ���� : Dest.Color = Dest * (1-Src.Color);			 	
	m_Device->CreateBlendState(&bd, &m_BState[BS_COLOR_BLEND]);


	//------------------------------
	// ȥ�հ�ü3 : ���� ȥ�� "��� ���� Blending 1:1" 
	//------------------------------
	bd.RenderTarget[0].BlendEnable = TRUE;							//���� ȥ�� ON! 	
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//���� ȥ�� ���� "��Ⱝ��" : ������ = Src.Color + Dest.Color 		
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;					//�ҽ�(����) ȥ�� ����, ���� ����  : Src.Color = Src * 1;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;					//Ÿ��(���) ȥ�� ����, ���� ���� ���� : Dest.Color = Dest * 1;			 	
	m_Device->CreateBlendState(&bd, &m_BState[BS_COLOR_BLEND_ONE]);
}

void D3DDevice::CreateSamplerState()
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));

	//�ؽ�ó ���� : ���� ���͸�
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = m_Af;
	//�Ӹ���	
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	sd.MipLODBias = 0;
	//���� �⺻�� ó��..
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.BorderColor[0] = 1;
	sd.BorderColor[1] = 1;
	sd.BorderColor[2] = 1;
	sd.BorderColor[3] = 1;

	//���÷� ��ü1 ����. (DX �⺻��)
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = m_Device->CreateSamplerState(&sd, &m_pSampler[SS_CLAMP]);
	//hr = m_Device->CreateSamplerState();
	//���÷� ��ü2 ����.
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

	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;			// �� �켱 ���ó��
	//UINT Flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;		// �� �켱 ��� ó��

#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG;							//����� ���� �ɼ� �߰�.
	Flags |= D3DCOMPILE_SKIP_OPTIMIZATION;				//����� ���� �ɼ� �߰�.
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
		// ���� ���
		TCHAR func[80] = L"";
		::mbstowcs(func, entrypoint, strlen(entrypoint));
		TCHAR sm[20] = L"";
		::mbstowcs(sm, shadermodel, strlen(shadermodel));

		//���̴� ���� �޼��� �б�.
		TCHAR errw[4096] = L"";
		::mbstowcs(errw, (char*)pError->GetBufferPointer(), pError->GetBufferSize());

		TCHAR herr[1024] = L"�Ʒ��� ������ Ȯ���Ͻʽÿ�.";

		TCHAR errmsg[1024];
		_stprintf(errmsg, L"%s \nFile=%s  Entry=%s  Target=%s  \n�����ڵ�(0x%08X) : %s \n\n%s",
			L"���̴� ������ ����", filename, func, sm,
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
		MessageBox(NULL, L"���� ���̴� �ε� ����", L"_Error", MB_OK | MB_ICONERROR);
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
		MessageBox(NULL, L"�ȼ� ���̴� �ε� ����", L"_Error", MB_OK | MB_ICONERROR);
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

	//����/���ٽ� ���ۿ� ���� ����.
	D3D11_TEXTURE2D_DESC   td;
	ZeroMemory(&td, sizeof(td));
	td.Width = m_Mode.Width;
	td.Height = m_Mode.Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	//td.Format = DXGI_FORMAT_D32_FLOAT;			// 32BIT. ���� ����.
	//td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// ���� ���� (24bit) + ���ٽ� (8bit) / ���� �ϵ���� (DX9)
	//td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// ���� ���� (32bit) + ���ٽ� (8bit) / ���� �ϵ���� (DX11)
	td.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;	// ���� ���� (32bit) + ���ٽ� (8bit) / ���� �ϵ���� (DX11)
	td.SampleDesc.Count = m_AA;						// AA ���� - RT �� ���� �԰� �ؼ�.
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;		// ����-���ٽ� ���ۿ����� ����.
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	// ���� ���� ����.
	//ID3D11Texture2D* pDS = NULL;						 
	hr = m_Device->CreateTexture2D(&td, NULL, &m_pDS);
	if (FAILED(hr))
	{
		return hr;
	}


	// ����-���ٽǹ��ۿ� ���ҽ� �� ���� ����. 
	D3D11_DEPTH_STENCIL_VIEW_DESC  dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Format = td.Format;
	//dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; //AA ����.
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;	//+AA ���� "MSAA"
	dd.Texture2D.MipSlice = 1;

	//����-���ٽ� ���� �� ����.
	hr = m_Device->CreateDepthStencilView(m_pDS, &dd, &m_pDSView);
	if (FAILED(hr))
	{
		return hr;
	}

	//���ҽ� �� ���� ��, ���ʿ��� DX �ڵ��� �����ؾ� �մϴ�.(�޸� ���� ����)
	//SAFE_RELEASE(pDS);

	return hr;
}

HRESULT D3DDevice::LoadTexture(string filename, ID3D11ShaderResourceView** ppTexRV)
{
	HRESULT hr = S_OK;

	ID3D11ShaderResourceView* pTexRV = nullptr;

	TCHAR* finalpath = ConvertStringToTCHAR(filename);
	//TCHAR* fileName = L"../data/sonim.jpg";

	//�Ϲ� �ؽ�ó �ε�.
	//hr = DirectX::CreateWICTextureFromFile( g_pDevice, fileName, nullptr, &g_pTextureRV );	

	//�Ӹ�+�ؽ�ó �ε�.
	hr = DirectX::CreateWICTextureFromFileEx(m_Device, m_DeviceContect, finalpath, 0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0, D3D11_RESOURCE_MISC_GENERATE_MIPS, WIC_LOADER_DEFAULT,
		nullptr, &pTexRV);

	if (FAILED(hr))
	{
		//DDS ���Ϸ� �ε� �õ�. : +�Ӹ�
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

	//�ؽ�ó �Ӹ����.
	//g_pDXDC->GenerateMips(g_pTextureRV);

	//�ܺη� ����.
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
		D3D11_RESOURCE_MISC_TEXTURECUBE,		// ť��� ���ҽ� ���� �ɼ�
		false, nullptr, &pTexRV);
	if (FAILED(hr))
	{
		return hr;
	}

	// �ܺη� ����
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
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// �뵵 : ���ؽ� ����
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
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;	// �뵵 : �ε��� ����
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
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// �뵵 : ���ؽ� ����
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
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;	// �뵵 : �ε��� ����
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

	//���̴� ���ҽ� �� ���� ȹ��.
	D3D11_SHADER_RESOURCE_VIEW_DESC dc;
	pRV->GetDesc(&dc);

	//�ؽ�ó ���� ȹ��.
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