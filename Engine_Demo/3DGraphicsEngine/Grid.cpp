//
//
//
// 디버깅용 그리드 
// [2020/08/06 RUNSUMIN]

#include "Grid.h"

Grid::Grid()
{
	m_pDeviceCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDeviceContextCopy = D3DDevice::GetInstance()->GetDeviceContext();
}

Grid::~Grid()
{
	m_pShader->ShaderRelease();
}

void Grid::InitializeGrid(int numx, int numy, float size)
{
	m_SizeX = numx;
	m_SizeY = numy;
	m_Length = size;
	m_Movegrid = Vector3(-(m_SizeX * m_Length / 2), 0, -(m_SizeY * m_Length / 2));
	InitGrid(numx, numy, size);
}

void Grid::UpdateGrid()
{
	Matrix view = D3DDevice::GetInstance()->m_View;
	Matrix proj = D3DDevice::GetInstance()->m_Proj;

	Update(view, proj);
}

void Grid::DrawGrid()
{
	Render();
}

void Grid::ReleaseGrid()
{
	Release();
}

void Grid::ChangeGridSize(int numx, int numy, float size)
{
	m_SizeX = numx;
	m_SizeY = numy;
	m_Length = size;
	m_Movegrid = Vector3(-(m_SizeX * m_Length / 2), 0, -(m_SizeY * m_Length / 2));

	SetGrid(m_SizeX, m_SizeY, m_Length);

	// 버퍼 해제 후 재생성...
	m_pVb->Release();
	m_pIb->Release();

	HRESULT res = S_OK;

	UINT gridsize = sizeof(VTX_GIRD) * m_Grid.size();

	res = D3DDevice::GetInstance()->CreateVB_Dynamic(m_Grid.data(), gridsize, &m_pVb);

	UINT indexsize = sizeof(int) * m_index.size();

	res = D3DDevice::GetInstance()->CreateIB_Dynamic(m_index.data(), indexsize, &m_pIb);
	//UINT gridsize = sizeof(VTX_GIRD) * m_Grid.size();

	//D3DDevice::GetInstance()->UpdateDynamicBuffer(m_pVb, m_Grid.data(), gridsize);

	//gridsize = sizeof(int) * m_index.size();

	//D3DDevice::GetInstance()->UpdateDynamicBuffer(m_pIb, m_index.data(), gridsize);
}

void Grid::SetGrid(int numx, int numy, float size)
{
	m_index.clear();
	m_Grid.clear();
	int index = 0;
	Vector4 color = Vector4(1, 1, 1, 1);

	for (int j = 0; j < numx; j++)
	{
		for (int i = 0; i < numy; i++)
		{
			// 1
			VTX_GIRD grid;
			grid.x = static_cast<float>(i) * size;
			grid.y = 0.f;
			grid.z = static_cast<float>(j + 1) * size;
			m_index.push_back(index);
			index++;
			grid.color = color;
			m_Grid.push_back(grid);

			grid.x = static_cast<float>(i + 1) * size;
			grid.y = 0.f;
			grid.z = static_cast<float>(j + 1) * size;
			m_index.push_back(index);
			index++;
			grid.color = color;
			m_Grid.push_back(grid);

			// 2
			grid.x = static_cast<float>(i + 1) * size;
			grid.y = 0.f;
			grid.z = static_cast<float>(j + 1) * size;
			m_index.push_back(index);
			index++;
			grid.color = color;
			m_Grid.push_back(grid);

			grid.x = static_cast<float>(i + 1) * size;
			grid.y = 0.f;
			grid.z = static_cast<float>(j) * size;
			m_index.push_back(index);
			index++;
			grid.color = color;
			m_Grid.push_back(grid);

			// 3
			grid.x = static_cast<float>(i + 1) * size;
			grid.y = 0.f;
			grid.z = static_cast<float>(j) * size;
			m_index.push_back(index);
			index++;
			grid.color = color;
			m_Grid.push_back(grid);

			grid.x = static_cast<float>(i) * size;
			grid.y = 0.f;
			grid.z = static_cast<float>(j) * size;
			m_index.push_back(index);
			index++;
			grid.color = color;
			m_Grid.push_back(grid);

			// 4
			grid.x = static_cast<float>(i) * size;
			grid.y = 0.f;
			grid.z = static_cast<float>(j) * size;
			m_index.push_back(index);
			index++;
			grid.color = color;
			m_Grid.push_back(grid);

			grid.x = static_cast<float>(i) * size;
			grid.y = 0.f;
			grid.z = static_cast<float>(j + 1) * size;
			m_index.push_back(index);
			index++;
			grid.color = color;
			m_Grid.push_back(grid);


		}
	}

}

int Grid::InitGrid(int numx, int numy, float size)
{
	SetGrid(numx, numy, size);

	HRESULT res = S_OK;

	UINT gridsize = sizeof(VTX_GIRD) * m_Grid.size();

	res = D3DDevice::GetInstance()->CreateVB_Dynamic(m_Grid.data(), gridsize, &m_pVb);

	UINT indexsize = sizeof(int) * m_index.size();

	res = D3DDevice::GetInstance()->CreateIB_Dynamic(m_index.data(), indexsize, &m_pIb);

	// 셰이더 로드
	m_pShader = new Shader();

	m_pShader->ShaderLoad(L"../shader/Line.fx", m_pDeviceCopy);

	/// 각 셰이더마다 다르게 정점 레이아웃 설정 가능(모델 & 상황포함)
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION",	   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);
	m_pShader->CreateVBLayOut(layout, numElements, m_pDeviceCopy);

	ZeroMemory(&m_cbGrid, sizeof(CBGRID));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBGRID), &m_cbGrid, &m_pGridVertexbuf, m_pDeviceCopy);

	// 셰이더 셋팅
	m_pDeviceContextCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDeviceContextCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	m_pDeviceContextCopy->VSSetConstantBuffers(0, 1, &m_pGridVertexbuf);

	return OK_SIGN;
}

int Grid::Update(Matrix view, Matrix proj)
{
	XMMATRIX mTm = XMMatrixIdentity();
	m_cbGrid.mWorld = XMMatrixTranslationFromVector(m_Movegrid);
	m_cbGrid.mView = view;
	m_cbGrid.mProj = proj;

	//최종 행렬 결합.
	m_cbGrid.mWVP = m_cbGrid.mWorld * m_cbGrid.mView * m_cbGrid.mProj;
	m_pShader->UpdateDynamicConstantBuffer(m_pDeviceContextCopy, m_pGridVertexbuf, &m_cbGrid, sizeof(CBGRID));

	return OK_SIGN;
}

int Grid::Render()
{
	UINT stride = sizeof(VTX_GIRD);
	UINT offset = 0;

	m_pDeviceContextCopy->IASetVertexBuffers(0, 1, &m_pVb, &stride, &offset);
	m_pDeviceContextCopy->IASetIndexBuffer(m_pIb, DXGI_FORMAT_R32_UINT, offset);
	m_pDeviceContextCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDeviceContextCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	//기본 셰이더 설정.
	m_pDeviceContextCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDeviceContextCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	m_pDeviceContextCopy->VSSetConstantBuffers(0, 1, &m_pGridVertexbuf);

	//기본 셰이더 상수버퍼 설정.
	//m_pDeviceContextCopy->OMSetRenderTargets(1, &D3DDevice::GetInstance()->m_RendertargetView, D3DDevice::GetInstance()->m_pDSView);

	//D3DDevice::GetInstance()->GetDeviceContext()->RSSetState(D3DDevice::GetInstance()->m_RState[RS_SOLID]);
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON_STENCIL_OFF], 0);
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetBlendState(D3DDevice::GetInstance()->m_BState[BS_DEFAULT], NULL, 0xFFFFFFFF);

	m_pDeviceContextCopy->DrawIndexed(m_Grid.size(), 0, 0);

	return OK_SIGN;
}

void Grid::Release()
{
	m_pShader->ShaderRelease();
	SAFE_RELEASE(m_pDeviceCopy);
	SAFE_RELEASE(m_pDeviceContextCopy);
	SAFE_RELEASE(m_pGridVertexbuf);
	SAFE_RELEASE(m_pVb);
}
