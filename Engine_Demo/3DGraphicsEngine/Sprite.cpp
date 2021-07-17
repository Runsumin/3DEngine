#include "Sprite.h"
#include "ShaderMgr.h"
Sprite::Sprite(Vector3 pos)
{
	// 장치 정보 받아오기
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();
	m_pShader = ShaderMgr::GetInstance()->m_pUI;
	m_pPsDepth = ShaderMgr::GetInstance()->m_pPsDepth_UI;

	ZeroMemory(&m_TexDesc, sizeof(D3D11_TEXTURE2D_DESC));
}

Sprite::Sprite(Vector3 pos, Vector3 scl, string TexFileName)
{
	// 장치 정보 받아오기
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();
	m_pShader = ShaderMgr::GetInstance()->m_pUI;
	m_pPsDepth = ShaderMgr::GetInstance()->m_pPsDepth_UI;
	m_Filename = TexFileName;

	m_View = XMMatrixIdentity();
	m_Porj = XMMatrixOrthographicOffCenterLH(0, (float)D3DDevice::GetInstance()->m_ScreenWidth,
		(float)D3DDevice::GetInstance()->m_ScreenHeight, 0.0f, 1.0f, 10000.f);

	m_Pos = pos;
	m_Scl = scl;
	m_ChangeSizeX = 1;
	m_ChangeSizeY = 1;
	m_OverRayCol = Vector4(1, 1, 1, 1);
}

Sprite::Sprite(Vector3 pos, Vector3 scl, ID3D11ShaderResourceView* ptex)
{
	// 장치 정보 받아오기
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();

	m_pShader = ShaderMgr::GetInstance()->m_pUI;
	m_pPsDepth = ShaderMgr::GetInstance()->m_pPsDepth_UI;
	m_pPsBloom = ShaderMgr::GetInstance()->m_pPsBloom;
	m_pPsFinal = ShaderMgr::GetInstance()->m_pPsFinal;
	m_pPsSampling = ShaderMgr::GetInstance()->m_pPsSampling;
	m_pPsBlur = ShaderMgr::GetInstance()->m_pPsBlur;

	m_View = XMMatrixIdentity();
	m_Porj = XMMatrixOrthographicOffCenterLH(0, (float)D3DDevice::GetInstance()->m_ScreenWidth,
		(float)D3DDevice::GetInstance()->m_ScreenHeight, 0.0f, 1.0f, 10000.f);

	m_Pos = pos;
	m_Scl = scl;
	m_ChangeSizeX = 1;
	m_ChangeSizeY = 1;
	m_OverRayCol = Vector4(1, 1, 1, 1);

	m_pTex = ptex;

	D3D11_TEXTURE2D_DESC td;
	D3DDevice::GetInstance()->GetTextureDesc(m_pTex, &td);
	m_TexDesc = td;
	m_Width = (float)td.Width;
	m_Height = (float)td.Height;
}

Sprite::~Sprite()
{

}

int Sprite::InitSprite()
{
	// 상수버퍼 정보 초기화
	ZeroMemory(&m_CbDef, sizeof(CBDEFAULT));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBDEFAULT), &m_CbDef, &m_pCbDefBuf, m_pDevCopy);

	ZeroMemory(&m_CbMtl, sizeof(OVERLAYCOLOR));
	m_pShader->CreateDynamicConstantBuffer(sizeof(OVERLAYCOLOR), &m_CbMtl, &m_pCbMtlBuf, m_pDevCopy);

	// 텍스쳐 로드
	if (m_pTex == NULL)
	{
		D3DDevice::GetInstance()->LoadTexture(m_Filename, &m_pTex);
		D3D11_TEXTURE2D_DESC td;
		D3DDevice::GetInstance()->GetTextureDesc(m_pTex, &td);
		m_TexDesc = td;
		m_Width = (float)td.Width;
		m_Height = (float)td.Height;
	}
	else
	{

	}

	_VTX	vtxs[] =
	{
		{ VECTOR3(0, 0, 0),  VECTOR2(0, 0)},
		{ VECTOR3(1, 0, 0),  VECTOR2(1, 0)},
		{ VECTOR3(0, 1, 0),  VECTOR2(0, 1)},
		{ VECTOR3(1, 1, 0),  VECTOR2(1, 1)},
	};

	D3DDevice::GetInstance()->CreateVB(vtxs, sizeof(vtxs), &m_pVB);



	return OK_SIGN;
}

int Sprite::InitShader()
{
	m_pShader = new Shader();

	m_pShader->ShaderLoad(L"../shader/UI.fx", m_pDevCopy);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	m_pShader->CreateVBLayOut(layout, numElements, m_pDevCopy);


	// 뎁스 버퍼 저장용 픽셀 셰이더 로드
	D3DDevice::GetInstance()->PixelShaderLoad(L"../shader/UI.fx", "PS_DEPTH", "ps_5_0", &m_pPsDepth);

	return OK_SIGN;
}

void Sprite::UpdateSprite(Vector3 pos, Vector3 scl)
{
	m_Pos = pos;
	m_Scl = scl;
}

int Sprite::DrawSprite()
{
	//////////////////////////////////////////////////////////////
	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_CLAMP]);
	m_pDevConCopy->RSSetState(D3DDevice::GetInstance()->m_RState[RS_SOLID]);
	m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_WRITE_OFF], 0);
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_ON], NULL, 0xFFFFFFFF);

	// 2d 스크린 좌표 보정
	m_Pos.x - 0.5f;
	m_Pos.y - 0.5f;

	// 스프라이트 변환
	float sizex = (float)m_Width * m_ChangeSizeX;								// 원본 스프라이트 크기(픽셀)
	float sizey = (float)m_Height * m_ChangeSizeY;
	Matrix mScale = XMMatrixScaling(sizex * m_Scl.x, sizey * m_Scl.y, 1.0f);
	Matrix mTrans = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);	// 위치 설정 : 2D Screen 좌표
	//XMMATRIX mRot = XMMatrixRotationY(vRot.y);
	Matrix mTM = mScale * mTrans;

	m_CbDef.mTm = mTM;
	m_CbDef.mView = m_View;
	m_CbDef.mProj = m_Porj;
	m_CbDef.mWV = mTM * m_View;
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbDefBuf, &m_CbDef, sizeof(CBDEFAULT));

	m_CbMtl.color = m_OverRayCol;

	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbMtlBuf, &m_CbMtl, sizeof(OVERLAYCOLOR));

	// draw
	UINT stride = sizeof(_VTX);
	UINT offset = 0;

	m_pDevConCopy->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pDevConCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDevConCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pCbDefBuf);			//상수 버퍼 설정.(VS)
	m_pDevConCopy->VSSetConstantBuffers(1, 1, &m_pCbMtlBuf);			//재질 정보 설정.(VS)

	m_pDevConCopy->PSSetConstantBuffers(0, 1, &m_pCbDefBuf);			//상수 버퍼 설정.(PS)
	m_pDevConCopy->PSSetConstantBuffers(1, 1, &m_pCbMtlBuf);			//재질 정보 설정.(PS)

																		//셰이더 리소스 설정.
	m_pDevConCopy->PSSetShaderResources(0, 1, &m_pTex);				//PS 에 텍스처 설정
	m_pDevConCopy->Draw(4, 0);

	///////////////////////////////////
	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_WRAP]);
	m_pDevConCopy->RSSetState(D3DDevice::GetInstance()->m_RState[RS_CULLBACK]);
	m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON], 0);

	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_DEFAULT], NULL, 0xFFFFFFFF);

	return OK_SIGN;
}

int Sprite::ReleaseSprite()
{
	SAFE_RELEASE(m_pDevCopy);
	SAFE_RELEASE(m_pDevConCopy);
	SAFE_DELETE(m_pCbDefBuf);
	SAFE_DELETE(m_pCbMtlBuf);
	SAFE_DELETE(m_pTex);
	m_pShader->ShaderRelease();

	return OK_SIGN;
}

void Sprite::SetChangeSize(float x, float y)
{
	m_ChangeSizeX = x;
	m_ChangeSizeY = y;

	// 버텍스 버퍼 해제 후 재생성.
	m_pVB->Release();

	_VTX	vtxs[] =
	{
		{ VECTOR3(0, 0, 0),  VECTOR2(0, 0)},
		{ VECTOR3(1, 0, 0),  VECTOR2(x, 0)},
		{ VECTOR3(0, 1, 0),  VECTOR2(0, y)},
		{ VECTOR3(1, 1, 0),  VECTOR2(x, y)},
	};

	D3DDevice::GetInstance()->CreateVB(vtxs, sizeof(vtxs), &m_pVB);
}

void Sprite::DrawSprite_Rt(ID3D11ShaderResourceView* ptex, float drawsizeX, float drawsizeY)
{
	//////////////////////////////////////////////////////////////
	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_CLAMP]);
	m_pDevConCopy->RSSetState(D3DDevice::GetInstance()->m_RState[RS_SOLID]);
	m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_WRITE_OFF], 0);
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_ON], NULL, 0xFFFFFFFF);

	// 2d 스크린 좌표 보정
	m_Pos.x - 0.5f;
	m_Pos.y - 0.5f;


	// 스프라이트 변환
	float sizex = (float)drawsizeX * m_ChangeSizeX;								// 원본 스프라이트 크기(픽셀)
	float sizey = (float)drawsizeY * m_ChangeSizeY;
	Matrix mScale = XMMatrixScaling(sizex * m_Scl.x, sizey * m_Scl.y, 1.0f);
	Matrix mTrans = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);	// 위치 설정 : 2D Screen 좌표
	//XMMATRIX mRot = XMMatrixRotationY(vRot.y);
	Matrix mTM = mScale * mTrans;

	m_CbDef.mTm = mTM;
	m_CbDef.mView = m_View;
	m_CbDef.mProj = m_Porj;
	m_CbDef.mWV = mTM * m_View;
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbDefBuf, &m_CbDef, sizeof(CBDEFAULT));

	m_CbMtl.color = m_OverRayCol;

	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbMtlBuf, &m_CbMtl, sizeof(OVERLAYCOLOR));

	// draw
	UINT stride = sizeof(_VTX);
	UINT offset = 0;

	m_pDevConCopy->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pDevConCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDevConCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pPsDepth, nullptr, 0);

	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pCbDefBuf);			//상수 버퍼 설정.(VS)
	m_pDevConCopy->VSSetConstantBuffers(1, 1, &m_pCbMtlBuf);			//재질 정보 설정.(VS)

	m_pDevConCopy->PSSetConstantBuffers(0, 1, &m_pCbDefBuf);			//상수 버퍼 설정.(PS)
	m_pDevConCopy->PSSetConstantBuffers(1, 1, &m_pCbMtlBuf);			//재질 정보 설정.(PS)

																		//셰이더 리소스 설정.
	m_pDevConCopy->PSSetShaderResources(0, 1, &ptex);				//PS 에 텍스처 설정
	m_pDevConCopy->Draw(4, 0);

	///////////////////////////////////
	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_WRAP]);
	m_pDevConCopy->PSSetSamplers(1, 1, &D3DDevice::GetInstance()->m_pSampler[SS_BOARDER]);
	m_pDevConCopy->RSSetState(D3DDevice::GetInstance()->m_RState[RS_CULLBACK]);
	m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON], 0);

	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_DEFAULT], NULL, 0xFFFFFFFF);

}

void Sprite::DrawSprite_Bloom(ID3D11ShaderResourceView* ptex, float drawsizeX, float drawsizeY)
{
	//////////////////////////////////////////////////////////////
	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_CLAMP]);
	m_pDevConCopy->RSSetState(D3DDevice::GetInstance()->m_RState[RS_SOLID]);
	m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_WRITE_OFF], 0);
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_ON], NULL, 0xFFFFFFFF);

	// 2d 스크린 좌표 보정
	m_Pos.x - 0.5f;
	m_Pos.y - 0.5f;

	// 스프라이트 변환
	float sizex = (float)drawsizeX * m_ChangeSizeX;								// 원본 스프라이트 크기(픽셀)
	float sizey = (float)drawsizeY * m_ChangeSizeY;
	Matrix mScale = XMMatrixScaling(sizex * m_Scl.x, sizey * m_Scl.y, 1.0f);
	Matrix mTrans = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);	// 위치 설정 : 2D Screen 좌표
	//XMMATRIX mRot = XMMatrixRotationY(vRot.y);
	Matrix mTM = mScale * mTrans;

	m_CbDef.mTm = mTM;
	m_CbDef.mView = m_View;
	m_CbDef.mProj = m_Porj;
	m_CbDef.mWV = mTM * m_View;
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbDefBuf, &m_CbDef, sizeof(CBDEFAULT));

	m_CbMtl.color = m_OverRayCol;

	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbMtlBuf, &m_CbMtl, sizeof(OVERLAYCOLOR));

	// draw
	UINT stride = sizeof(_VTX);
	UINT offset = 0;

	m_pDevConCopy->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pDevConCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDevConCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pPsBloom, nullptr, 0);

	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pCbDefBuf);			//상수 버퍼 설정.(VS)
	m_pDevConCopy->VSSetConstantBuffers(1, 1, &m_pCbMtlBuf);			//재질 정보 설정.(VS)

	m_pDevConCopy->PSSetConstantBuffers(0, 1, &m_pCbDefBuf);			//상수 버퍼 설정.(PS)
	m_pDevConCopy->PSSetConstantBuffers(1, 1, &m_pCbMtlBuf);			//재질 정보 설정.(PS)

																		//셰이더 리소스 설정.
	m_pDevConCopy->PSSetShaderResources(0, 1, &ptex);				//PS 에 텍스처 설정
	m_pDevConCopy->Draw(4, 0);

	///////////////////////////////////
	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_WRAP]);
	m_pDevConCopy->PSSetSamplers(1, 1, &D3DDevice::GetInstance()->m_pSampler[SS_BOARDER]);
	m_pDevConCopy->RSSetState(D3DDevice::GetInstance()->m_RState[RS_CULLBACK]);
	m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON], 0);

	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_DEFAULT], NULL, 0xFFFFFFFF);

}

void Sprite::DrawSprite_FINAL(ID3D11ShaderResourceView* pResult, ID3D11ShaderResourceView* pAddTex, 
	float drawsizeX, float drawsizeY)
{
	//////////////////////////////////////////////////////////////
	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_CLAMP]);
	m_pDevConCopy->RSSetState(D3DDevice::GetInstance()->m_RState[RS_SOLID]);
	m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_WRITE_OFF], 0);
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_ON], NULL, 0xFFFFFFFF);

	// 2d 스크린 좌표 보정
	m_Pos.x - 0.5f;
	m_Pos.y - 0.5f;

	// 스프라이트 변환
	float sizex = (float)drawsizeX * m_ChangeSizeX;								// 원본 스프라이트 크기(픽셀)
	float sizey = (float)drawsizeY * m_ChangeSizeY;
	Matrix mScale = XMMatrixScaling(sizex * m_Scl.x, sizey * m_Scl.y, 1.0f);
	Matrix mTrans = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);	// 위치 설정 : 2D Screen 좌표
	//XMMATRIX mRot = XMMatrixRotationY(vRot.y);
	Matrix mTM = mScale * mTrans;

	m_CbDef.mTm = mTM;
	m_CbDef.mView = m_View;
	m_CbDef.mProj = m_Porj;
	m_CbDef.mWV = mTM * m_View;
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbDefBuf, &m_CbDef, sizeof(CBDEFAULT));

	m_CbMtl.color = m_OverRayCol;
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbMtlBuf, &m_CbMtl, sizeof(OVERLAYCOLOR));

	// draw
	UINT stride = sizeof(_VTX);
	UINT offset = 0;

	m_pDevConCopy->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pDevConCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDevConCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pPsFinal, nullptr, 0);

	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pCbDefBuf);			//상수 버퍼 설정.(VS)
	m_pDevConCopy->VSSetConstantBuffers(1, 1, &m_pCbMtlBuf);			//재질 정보 설정.(VS)

	m_pDevConCopy->PSSetConstantBuffers(0, 1, &m_pCbDefBuf);			//상수 버퍼 설정.(PS)
	m_pDevConCopy->PSSetConstantBuffers(1, 1, &m_pCbMtlBuf);			//재질 정보 설정.(PS)

																		//셰이더 리소스 설정.
	m_pDevConCopy->PSSetShaderResources(0, 1, &pResult);				//PS 에 텍스처 설정
	m_pDevConCopy->PSSetShaderResources(1, 1, &pAddTex);				//PS 에 텍스처 설정
	m_pDevConCopy->Draw(4, 0);

	///////////////////////////////////
	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_WRAP]);
	m_pDevConCopy->PSSetSamplers(1, 1, &D3DDevice::GetInstance()->m_pSampler[SS_BOARDER]);
	m_pDevConCopy->RSSetState(D3DDevice::GetInstance()->m_RState[RS_CULLBACK]);
	m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON], 0);

	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_DEFAULT], NULL, 0xFFFFFFFF);

}

void Sprite::DrawSprite_Blur_ACC(ID3D11ShaderResourceView* pResult, ID3D11ShaderResourceView* pAddTex, float drawsizeX, float drawsizeY)
{
	//////////////////////////////////////////////////////////////
	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_CLAMP]);
	m_pDevConCopy->RSSetState(D3DDevice::GetInstance()->m_RState[RS_SOLID]);
	m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_WRITE_OFF], 0);
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_ON], NULL, 0xFFFFFFFF);

	// 2d 스크린 좌표 보정
	m_Pos.x - 0.5f;
	m_Pos.y - 0.5f;

	// 스프라이트 변환
	float sizex = (float)drawsizeX * m_ChangeSizeX;								// 원본 스프라이트 크기(픽셀)
	float sizey = (float)drawsizeY * m_ChangeSizeY;
	Matrix mScale = XMMatrixScaling(sizex * m_Scl.x, sizey * m_Scl.y, 1.0f);
	Matrix mTrans = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);	// 위치 설정 : 2D Screen 좌표
	//XMMATRIX mRot = XMMatrixRotationY(vRot.y);
	Matrix mTM = mScale * mTrans;

	m_CbDef.mTm = mTM;
	m_CbDef.mView = m_View;
	m_CbDef.mProj = m_Porj;
	m_CbDef.mWV = mTM * m_View;
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbDefBuf, &m_CbDef, sizeof(CBDEFAULT));

	m_CbMtl.color = m_OverRayCol;
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbMtlBuf, &m_CbMtl, sizeof(OVERLAYCOLOR));

	// draw
	UINT stride = sizeof(_VTX);
	UINT offset = 0;

	m_pDevConCopy->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pDevConCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDevConCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pPsSampling, nullptr, 0);

	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pCbDefBuf);			//상수 버퍼 설정.(VS)
	m_pDevConCopy->VSSetConstantBuffers(1, 1, &m_pCbMtlBuf);			//재질 정보 설정.(VS)

	m_pDevConCopy->PSSetConstantBuffers(0, 1, &m_pCbDefBuf);			//상수 버퍼 설정.(PS)
	m_pDevConCopy->PSSetConstantBuffers(1, 1, &m_pCbMtlBuf);			//재질 정보 설정.(PS)

																		//셰이더 리소스 설정.
	m_pDevConCopy->PSSetShaderResources(0, 1, &pResult);				//PS 에 텍스처 설정
	m_pDevConCopy->PSSetShaderResources(1, 1, &pAddTex);				//PS 에 텍스처 설정
	m_pDevConCopy->Draw(4, 0);

	///////////////////////////////////
	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_WRAP]);
	m_pDevConCopy->PSSetSamplers(1, 1, &D3DDevice::GetInstance()->m_pSampler[SS_BOARDER]);
	m_pDevConCopy->RSSetState(D3DDevice::GetInstance()->m_RState[RS_CULLBACK]);
	m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON], 0);

	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_DEFAULT], NULL, 0xFFFFFFFF);
}

void Sprite::DrawSprite_Blur(ID3D11ShaderResourceView* pResult, float drawsizeX, float drawsizeY, float SampleCount)
{
	//////////////////////////////////////////////////////////////
	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_CLAMP]);
	m_pDevConCopy->RSSetState(D3DDevice::GetInstance()->m_RState[RS_SOLID]);
	m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_WRITE_OFF], 0);
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_AB_ON], NULL, 0xFFFFFFFF);

	// 2d 스크린 좌표 보정
	m_Pos.x - 0.5f;
	m_Pos.y - 0.5f;

	// 스프라이트 변환
	float sizex = (float)drawsizeX * m_ChangeSizeX;								// 원본 스프라이트 크기(픽셀)
	float sizey = (float)drawsizeY * m_ChangeSizeY;
	Matrix mScale = XMMatrixScaling(sizex * m_Scl.x, sizey * m_Scl.y, 1.0f);
	Matrix mTrans = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);	// 위치 설정 : 2D Screen 좌표
	//XMMATRIX mRot = XMMatrixRotationY(vRot.y);
	Matrix mTM = mScale * mTrans;

	m_CbDef.mTm = mTM;
	m_CbDef.mView = m_View;
	m_CbDef.mProj = m_Porj;
	m_CbDef.mWV = mTM * m_View;
	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbDefBuf, &m_CbDef, sizeof(CBDEFAULT));

	m_CbMtl.color = m_OverRayCol;

	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbMtlBuf, &m_CbMtl, sizeof(OVERLAYCOLOR));

	// draw
	UINT stride = sizeof(_VTX);
	UINT offset = 0;

	m_pDevConCopy->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pDevConCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDevConCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pPsBlur, nullptr, 0);

	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pCbDefBuf);			//상수 버퍼 설정.(VS)
	m_pDevConCopy->VSSetConstantBuffers(1, 1, &m_pCbMtlBuf);			//재질 정보 설정.(VS)

	m_pDevConCopy->PSSetConstantBuffers(0, 1, &m_pCbDefBuf);			//상수 버퍼 설정.(PS)
	m_pDevConCopy->PSSetConstantBuffers(1, 1, &m_pCbMtlBuf);			//재질 정보 설정.(PS)

																		//셰이더 리소스 설정.
	m_pDevConCopy->PSSetShaderResources(0, 1, &pResult);				//PS 에 텍스처 설정
	m_pDevConCopy->Draw(4, 0);

	///////////////////////////////////
	m_pDevConCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_WRAP]);
	m_pDevConCopy->PSSetSamplers(1, 1, &D3DDevice::GetInstance()->m_pSampler[SS_BOARDER]);
	m_pDevConCopy->RSSetState(D3DDevice::GetInstance()->m_RState[RS_CULLBACK]);
	m_pDevConCopy->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON], 0);

	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_DEFAULT], NULL, 0xFFFFFFFF);
}
