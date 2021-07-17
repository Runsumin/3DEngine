///
///
/// 그래픽스 엔진 내부 셰이더 관리 클래스
/// 개요 : 엔진 내부애서 사용할 셰이더들을 미리 로드 해 놓고 
///			유동적으로 사용하기 위해...
/// 
/// [2020/12/30 RUNSUMIN]
#include "Shader.h"
#include "Fog.h"
#pragma once
class ShaderMgr
{
public:
	// 엔진 내부에서 생성할 필요가 있다.
	ShaderMgr();
	~ShaderMgr();
	// 매니저는 엔진 내부에서 싱글턴으로 동작한다.
	static ShaderMgr* m_Instance;
	static ShaderMgr* GetInstance();

public:
	enum class ShaderSort
	{
		AXIS, TERRAIN, BUILDING, CHARACTER, UI
	};

	// Static Mesh
	Shader* m_pStatic;
	// SKinned Mesh
	Shader* m_pSkinned;
	//// UI
	Shader* m_pUI;

	/// 뎁스 픽셀 셰이더
	ID3D11PixelShader* m_pPsDepth;
	ID3D11PixelShader* m_pPsDepth_Static;
	ID3D11PixelShader* m_pPsShadow;
	ID3D11PixelShader* m_pPsDepth_UI;
	ID3D11PixelShader* m_pPsBloom;
	ID3D11PixelShader* m_pPsFinal;
	ID3D11PixelShader* m_pPsSampling;
	ID3D11PixelShader* m_pPsBlur;
	//Depth 정보 셰이더 리소스...
	ID3D11ShaderResourceView* m_pShadow;

	// Skybox texture
	ID3D11ShaderResourceView* m_pSkybox;
	ID3D11ShaderResourceView* m_pSkybox_irradiance;

	// Direction Light ViewProjMatrix
	Matrix m_LightViewProjMat;
	CBFOG m_Fog;

	/// Channel View
	CHANNELVIEW m_ChannelView;

private:
	ID3D11Device* m_pDevCopy;				// 디바이스 카피
	ID3D11DeviceContext* m_pDevConCopy;		// 디바이스 컨텍스트 카피

public:
	void InitShaderMgr();
	void ReleaseShaderMgr();

};

