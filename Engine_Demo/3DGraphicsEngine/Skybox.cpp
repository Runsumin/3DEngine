#include "Skybox.h"
#include "ShaderMgr.h"
Skybox::Skybox(string filename)
{
	m_pDeviceCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDeviceContextCopy = D3DDevice::GetInstance()->GetDeviceContext();
	m_filename = filename;
}

Skybox::~Skybox()
{

}

void Skybox::InitSkybox()
{
	Vertex_Skybox MeshSkyBox[] = {
	Vector2(-1.0f,	 1.0f),
	Vector2(1.0f,  1.0f),
	Vector2(-1.0f, -1.0f),
	Vector2(1.0f, -1.0f)
	};

	HRESULT res = S_OK;

	res = D3DDevice::GetInstance()->CreateVB(MeshSkyBox, sizeof(MeshSkyBox) , &m_pVb);

	// Shader Init
	m_pShader = new Shader();

	m_pShader->ShaderLoad(L"../shader/SkyBox.fx", m_pDeviceCopy);
	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,  0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(layout);
	m_pShader->CreateVBLayOut(layout, numElements, m_pDeviceCopy);

	ZeroMemory(&m_SkyboxCB, sizeof(CBSKYBOX));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBSKYBOX), &m_SkyboxCB, &m_pSkyboxVertexbuf, m_pDeviceCopy);

	ZeroMemory(&m_Material, sizeof(Material));
	m_pShader->CreateDynamicConstantBuffer(sizeof(Material), &m_Material, &m_pMatBuf, m_pDeviceCopy);

	ZeroMemory(&m_CBFog, sizeof(CBFOG));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBFOG), &m_CBFog, &m_pFogBuf, m_pDeviceCopy);

	// 셰이더 셋팅
	m_pDeviceContextCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDeviceContextCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	m_pDeviceContextCopy->VSSetConstantBuffers(0, 1, &m_pSkyboxVertexbuf);

	D3DDevice::GetInstance()->LoadCubeTexture(m_filename, &m_Texture_ddsSkybox);

	string LoadName = "..\\Resource\\Skybox\\SkyBox3_1_irradiance.dds";
	D3DDevice::GetInstance()->LoadCubeTexture(m_filename, &m_Texture_ddsSkybox_irradiance);

	ShaderMgr::GetInstance()->m_pSkybox = m_Texture_ddsSkybox;
	ShaderMgr::GetInstance()->m_pSkybox_irradiance = m_Texture_ddsSkybox_irradiance;

	m_Material.Color = Vector4(1, 1, 1, 1);
}

void Skybox::DrawSkybox()
{
	m_CBFog = ShaderMgr::GetInstance()->m_Fog;
	m_pDeviceContextCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_OFF], 0);

	Matrix view = D3DDevice::GetInstance()->m_View;
	Matrix proj = D3DDevice::GetInstance()->m_Proj;

	m_SkyboxCB.mView_inv = XMMatrixInverse(NULL, view);
	m_SkyboxCB.mProj_inv = XMMatrixInverse(NULL, proj);
	m_SkyboxCB.m_View = view;
	m_pShader->UpdateDynamicConstantBuffer(m_pDeviceContextCopy, m_pSkyboxVertexbuf, &m_SkyboxCB, sizeof(CBSKYBOX));
	m_pShader->UpdateDynamicConstantBuffer(m_pDeviceContextCopy, m_pMatBuf, &m_Material, sizeof(Material));

	UINT stride = sizeof(Vertex_Skybox);
	UINT offset = 0;

	m_pDeviceContextCopy->IASetVertexBuffers(0, 1, &m_pVb, &stride, &offset);
	m_pDeviceContextCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDeviceContextCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_pDeviceContextCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_CLAMP]);

	//기본 셰이더 설정.
	m_pDeviceContextCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDeviceContextCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	m_pDeviceContextCopy->VSSetConstantBuffers(0, 1, &m_pSkyboxVertexbuf);
	m_pDeviceContextCopy->PSSetConstantBuffers(1, 1, &m_pMatBuf);
	m_pDeviceContextCopy->PSSetShaderResources(0, 1, &m_Texture_ddsSkybox);	//PS 에 텍스처 설정.

	m_pDeviceContextCopy->Draw(4, 0);

	m_pDeviceContextCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON], 0);



}

void Skybox::ChangeSkyboxColor(Vector4 Color)
{
	m_Material.Color = Color;
}

void Skybox::ReleaseSkybox()
{

}
