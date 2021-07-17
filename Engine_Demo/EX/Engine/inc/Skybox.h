///
///
///
/// Skybox - 
/// [2021/04/29 RUNSUMIN]
#pragma once
#include "DxDefine.h"
#include "Shader.h"
#include "EngineInterface.h"

class Skybox : public GRAPHICENGINE::ISkyBoxInterface
{
public:
	Skybox(string filename);
	~Skybox();
public:
	struct Vertex_Skybox
	{
		Vector2 pos;
	};

	struct Material
	{
		Vector4 Color;
	};

	_declspec(align(16)) struct CBSKYBOX
	{
		XMMATRIX mView_inv;
		XMMATRIX mProj_inv;
		XMMATRIX m_View;
	};
private:
	// device copy
	ID3D11Device* m_pDeviceCopy;
	ID3D11DeviceContext* m_pDeviceContextCopy;
	
	string m_filename;
	// Shader
	Shader* m_pShader;

	// Constant Buffer
	CBSKYBOX m_SkyboxCB;
	Material m_Material;

	// Buffer
	ID3D11Buffer* m_pVb;
	ID3D11Buffer* m_pSkyboxVertexbuf;
	ID3D11Buffer* m_pMatBuf;
	ID3D11Buffer* m_pFogBuf;
	// Resource
	ID3D11ShaderResourceView* m_Texture_ddsSkybox;
	ID3D11ShaderResourceView* m_Texture_ddsSkybox_irradiance;
	CBFOG m_CBFog;

public:
	virtual void InitSkybox() override;
	virtual void DrawSkybox() override;
	virtual void ChangeSkyboxColor(Vector4 Color) override;
	virtual void ReleaseSkybox() override;
};

