#include "Axis.h"

Axis::Axis()
{
	m_pDeviceCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDeviceContextCopy = D3DDevice::GetInstance()->GetDeviceContext();
}

Axis::~Axis()
{

}

int Axis::InitAxis()
{
	float thickness = 0.5f;
	Vector3 startposx = Vector3(0, 0, 0);
	Vector3 endposx = Vector3(3, 0, 0);
	VTX_AXIS m_Axis[] =
	{
		// Line 1
		{0,0,0, 1,0,0,1}, //x red
		{3,0,0, 1,0,0,1}, //x red
		// Line 2
		{0,0,0, 0,1,0,1}, //y Green
		{0,3,0, 0,1,0,1}, //y Green
		// Line 3
		{0,0,0, 0,0,1,1}, //z Blue
		{0,0,3, 0,0,1,1}, //z Blue
	};

	HRESULT res = S_OK;

	res = D3DDevice::GetInstance()->CreateVB(m_Axis, sizeof(m_Axis), &m_pVb);

	// 셰이더 로드

	m_pShader = new Shader();

	m_pShader->ShaderLoad(L"../shader/Axis.fx", m_pDeviceCopy);

	/// 각 셰이더마다 다르게 정점 레이아웃 설정 가능(모델 & 상황포함)
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION",	   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);
	m_pShader->CreateVBLayOut(layout, numElements, m_pDeviceCopy);

	ZeroMemory(&m_cbAxis, sizeof(CBAXIS));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBAXIS), &m_cbAxis, &m_pAxisbuf, m_pDeviceCopy);

	// 셰이더 셋팅
	m_pDeviceContextCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDeviceContextCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	m_pDeviceContextCopy->VSSetConstantBuffers(0, 1, &m_pAxisbuf);

	return OK_SIGN;
}

int Axis::Update(Matrix world)
{
	m_cbAxis.mView = D3DDevice::GetInstance()->m_View;
	m_cbAxis.mProj = D3DDevice::GetInstance()->m_Proj;

	m_cbAxis.mWorld = world;

	//최종 행렬 결합.
	m_cbAxis.mWVP = m_cbAxis.mWorld * m_cbAxis.mView * m_cbAxis.mProj;
	m_pShader->UpdateDynamicConstantBuffer(D3DDevice::GetInstance()->GetDeviceContext(), m_pAxisbuf, &m_cbAxis, sizeof(CBAXIS));

	return OK_SIGN;
}

int Axis::Render()
{
	UINT stride = sizeof(VTX_AXIS);
	UINT offset = 0;

	m_pDeviceContextCopy->IASetVertexBuffers(0, 1, &m_pVb, &stride, &offset);

	m_pDeviceContextCopy->VSSetConstantBuffers(0, 1, &m_pAxisbuf);
	m_pDeviceContextCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDeviceContextCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	//기본 셰이더 설정.
	m_pDeviceContextCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDeviceContextCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	//기본 셰이더 상수버퍼 설정.
	D3DDevice::GetInstance()->GetDeviceContext()->RSSetState(D3DDevice::GetInstance()->m_RState[RS_SOLID]);
	m_pDeviceContextCopy->OMSetRenderTargets(1, &D3DDevice::GetInstance()->m_RendertargetView, D3DDevice::GetInstance()->m_pDSView);
	D3DDevice::GetInstance()->GetDeviceContext()->RSSetState(D3DDevice::GetInstance()->m_RState[RS_CULLBACK]);

	//D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_DEFAULT], NULL, 0xFFFFFFFF);

	m_pDeviceContextCopy->Draw(6, 0);
	return OK_SIGN;
}

void Axis::Release()
{
	SAFE_RELEASE(m_pDeviceCopy);
	SAFE_RELEASE(m_pDeviceContextCopy);
	//m_pShader->ShaderRelease();
	SAFE_RELEASE(m_pAxisbuf);
	SAFE_RELEASE(m_pVb);

}
