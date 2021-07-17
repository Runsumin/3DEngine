//
//
//
// Dx11 초기화
// [2020/07/23 RUNSUMIN]
#pragma once

#include "DxDefine.h"
#include "Camera.h"
#include "EngineInterface.h"

/// 스프라이트에서 사용중인데 추후 삭제 예정...
/// 상수버퍼 (16바이트 정렬)
_declspec(align(16)) struct CBDEFAULT
{
	XMMATRIX mTm;
	XMMATRIX mView;
	XMMATRIX mProj;
	XMMATRIX mWV;
	XMMATRIX mWVP;

	XMMATRIX m_BoneTm[100];
	XMMATRIX mUserTm;
	BOOL	 bSkin;
};

/// 재질정보
_declspec(align(16)) struct CBMATERIAL
{
	COLOR mDiffuse;
	COLOR mAmbient;
	COLOR mSpecular;
	FLOAT mSpecPower;
	BOOL  bTexOn;

};

/////////////////////////////////////////////////////////////////

/// 상수버퍼_FBX 전용
_declspec(align(16)) struct CBFBXDEF
{
	Matrix mTm;
	Matrix mWorld;
	Matrix mView;
	Matrix mProj;
	Matrix mWV;
	Matrix mWVP;
	Matrix m_BoneTm[100];
	Matrix m_AxisChange;
	Matrix m_LightViewProj;
	Vector3 CamWorldPos;
};

_declspec(align(16)) struct CBFUPDATEONCE
{
	Matrix mView;
	Matrix mProj;
	Matrix m_AxisChange;
	///etc....
};

/// 재질정보_FBX 전용...
_declspec(align(16)) struct CBFBXMAT
{
	Vector4 Diffuse;
	Vector4 Ambient;
	Vector4 Specular;
	BOOL TexOn;
	BOOL NormalOn;
	BOOL SkinOn;
	BOOL MaskOn;
	BOOL BumpMappingOn;
	BOOL PBROn;
	BOOL EmissiveOn;
};

/// 광원정보_FBX 전용
_declspec(align(16)) struct CBLIGHT
{
	Vector4 direction;
	Vector4	diffuse;
	Vector4 ambient;
};

/// 점광원정보_
_declspec(align(16)) struct CBPOINTLIGHT
{
	Vector4 diffuse;
	Vector4	ambient;
	Vector3 position;
	float range;
};

/// 안개 정보
_declspec(align(16)) struct CBFOG
{
	Vector4 Color;
	float Density;
	BOOL OnOff;
};

/// 안개 정보
_declspec(align(16)) struct CHANNELVIEW
{
	BOOL Default;
	BOOL Diffuse;
	BOOL Normal;
	BOOL AmbientOcclusion;
	BOOL Roughness;
	BOOL Metalic;
	BOOL ShadowMap;
	BOOL IBL_SkyBox;
};

class D3DDevice
{
public:
	D3DDevice();
	~D3DDevice();
	// 디바이스는 엔진 내부에서 싱글턴으로 동작한다.
	static D3DDevice* m_Instance;
	static D3DDevice* GetInstance();

public:
	// 장치 초기화 관련
	IDXGISwapChain* m_SwapChain;
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContect;
	ID3D11RenderTargetView* m_RendertargetView;
	DXGI_MODE_DESC	m_Mode;
	D3D_FEATURE_LEVEL m_FeatureLevels;
	ID3D11Texture2D* m_pDS;					//깊이-스텐실 버퍼.
	ID3D11DepthStencilView* m_pDSView;		//깊이-스텐실 뷰.

	
	// 깊이 스텐실 버퍼 상태 객체
	ID3D11DepthStencilState* m_DSState[eDepthStencilSet::DS_MAX_];
	// 렌더링 상태 객체
	ID3D11RasterizerState* m_RState[eObjectRasterizerState::RS_MAX_];
	// 색상 혼합(블랜딩) 상태 객체
	ID3D11BlendState* m_BState[eObjectBlendingState::BS_MAX_];
	// 텍스쳐 샘플러 상태 객체
	ID3D11SamplerState* m_pSampler[eTextureSamplerState::SS_MAX];

public:
	// 화면 해상도
	float m_ScreenWidth;
	float m_ScreenHeight;

	// AA, AF 옵션
	DWORD m_AA;
	DWORD m_Af;

	// 수직 동기화
	bool m_Vsync;

	// 카메라 메트릭스(임시).. 어디로 가야 하오...
	Matrix m_View;
	Matrix m_Proj;

	// 라이트(임시)
	CBLIGHT m_DirectionLight;

public:
	/// 생성
	bool SetDXGI_Mode(HWND hwnd);
	HRESULT CreateDeviceSwapChain(HWND hwnd);
	HRESULT CreateRenderTarget();
	HRESULT CreateDepthStencil();
	HRESULT LoadTexture(string filename, ID3D11ShaderResourceView** ppTexRV);
	HRESULT LoadCubeTexture(string filename, ID3D11ShaderResourceView** ppTexRV);
	HRESULT CreateVB(void* pBuff, UINT size, ID3D11Buffer** ppVB);
	HRESULT CreateIB(void* pBuff, UINT size, ID3D11Buffer** ppIB);
	HRESULT CreateVB_Dynamic(void* pBuff, UINT size, ID3D11Buffer** ppVB);
	HRESULT CreateIB_Dynamic(void* pBuff, UINT size, ID3D11Buffer** ppIB);
	HRESULT UpdateDynamicBuffer(ID3D11Resource* pBuff, LPVOID pData, UINT size);
	HRESULT GetTextureDesc(ID3D11ShaderResourceView* pRV, D3D11_TEXTURE2D_DESC* pDesc);

	/// 엔진 밖에서 쓰여질 부분
	bool EngineSetUp(HWND hwnd); // Dx device 초기화
	void BeginRend(Vector4 backbuffercolor);// Draw 시작
	void EndRend();	// Draw 끝
	float GetEngineTime();
	float GetFramePerSecond();
	void Release();
	void ChangeRenderState(GRAPHICENGINE::RenderState renderstate);
	void GetCameraMatrixOnce(Matrix view, Matrix proj);
	void GetDirectionLightData(Vector4 Direction, Vector4 Diffuse, Vector4 Ambient);
	
	CBLIGHT GiveDirectionLightData();

	///
	/// 엔진 내부에서 쓰여질 부분
	void ClearBackBufer(UINT flag, float depth, UINT stencil, Vector4 col);
	void Flip();
	void SetViewport();
	void SetViewport(float Screenwidth, float screenheight);

	void CreateDepthStencilState();
	void CreateRasterState();
	void CreateBlenderState();
	void CreateSamplerState();

	/// Shader

	HRESULT ShaderCompile(const TCHAR* filename, const char* entrypoint, const char* shadermodel,
		ID3DBlob** ppcode);

	// 버텍스 셰이더 로드
	HRESULT VertexShaderLoad(const TCHAR* filename, const char* entrypoint, const char* shadermodel,
		ID3D11VertexShader** pvs, ID3DBlob** pvscode);

	// 픽셀 셰이더 로드
	HRESULT PixelShaderLoad(const TCHAR* filename, const char* entrypoint, const char* shadermodel,
		ID3D11PixelShader** pps);

	/// 문자열
	TCHAR* ConvertStringToTCHAR(string& s);
	string ConvertTCharToString(const TCHAR* ptsz);

/// Get, Set
public:
	ID3D11Device* GetDevice() { return m_Device; }
	ID3D11DeviceContext* GetDeviceContext() { return m_DeviceContect; }
	// 셋팅
	void SetDepthStencilState(ID3D11DepthStencilState* state, int refcnt);
	void SetRenderingState(ID3D11RasterizerState* state);
	void SetBlendingState(ID3D11BlendState* state);
	void SetSamplerState(ID3D11SamplerState* state);


};

