#include "Geometry_Generater.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Box::Box(float width, float height, float depth, Vector4 color)
{
	// 장치 정보 받아오기
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();

	m_Width = width;
	m_Height = height;
	m_Depth = depth;
	m_Color = color;
}
Box::~Box()
{
	Release();
}

void Box::Initialize()
{
	Vector4 Color = m_Color;
	float width2 = 0.5f * m_Width;
	float height2 = 0.5f * m_Height;
	float depth2 = 0.5f * m_Depth;

	Vertex m_Box[] =
	{
		// front
		{Vector3(-width2, -height2,-depth2),Color},
		{Vector3(-width2, +height2,-depth2),Color},
		{Vector3(+width2, +height2,-depth2),Color},
		{Vector3(+width2, -height2,-depth2),Color},
		// back
		{Vector3(-width2, -height2,+depth2),Color},
		{Vector3(+width2, -height2,+depth2),Color},
		{Vector3(+width2, +height2,+depth2),Color},
		{Vector3(-width2, +height2,+depth2),Color},
		// top
		{Vector3(-width2, +height2,-depth2),Color},
		{Vector3(-width2, +height2,+depth2),Color},
		{Vector3(+width2, +height2,+depth2),Color},
		{Vector3(+width2, +height2,-depth2),Color},
		// bottom
		{Vector3(-width2, -height2,-depth2),Color},
		{Vector3(+width2, -height2,-depth2),Color},
		{Vector3(+width2, -height2,+depth2),Color},
		{Vector3(-width2, -height2,+depth2),Color},
		// left
		{Vector3(-width2, -height2,+depth2),Color},
		{Vector3(-width2, +height2,+depth2),Color},
		{Vector3(-width2, +height2,-depth2),Color},
		{Vector3(-width2, -height2,-depth2),Color},
		// right
		{Vector3(+width2, -height2,-depth2),Color},
		{Vector3(+width2, +height2,-depth2),Color},
		{Vector3(+width2, +height2,+depth2),Color},
		{Vector3(+width2, -height2,+depth2),Color},

	};

	Index m_index[]{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	HRESULT res = S_OK;

	res = D3DDevice::GetInstance()->CreateVB(m_Box, sizeof(m_Box), &m_pVB);

	res = D3DDevice::GetInstance()->CreateIB(m_index, sizeof(m_index), &m_pIB);

	m_pShader = new Shader();

	m_pShader->ShaderLoad(L"../shader/Box.fx", m_pDevCopy);

	/// 각 셰이더마다 다르게 정점 레이아웃 설정 가능(모델 & 상황포함)
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION",	   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);
	m_pShader->CreateVBLayOut(layout, numElements, m_pDevCopy);

	ZeroMemory(&m_cbData, sizeof(CBGEOMTRY));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBGEOMTRY), &m_cbData, &m_pCbVertex, m_pDevCopy);

	// 셰이더 셋팅
	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);
}

void Box::Update(Matrix World)
{
	m_cbData.mView = D3DDevice::GetInstance()->m_View;
	m_cbData.mProj = D3DDevice::GetInstance()->m_Proj;
	m_cbData.mWorld = World;

}

void Box::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	m_pDevConCopy->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pDevConCopy->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, offset);
	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pCbVertex);
	m_pDevConCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDevConCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbVertex, &m_cbData, sizeof(CBGEOMTRY));

	//기본 셰이더 설정.
	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	m_pDevConCopy->DrawIndexed(36, 0, 0);
}

void Box::Release()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pDevCopy);
	SAFE_RELEASE(m_pDevConCopy);
	SAFE_RELEASE(m_pCbVertex);
	m_pShader->ShaderRelease();
}

void Box::ChangeColor(Vector4 col)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sphere::Sphere(float radious, int slicecount, int stackcount, Vector4 color)
	:m_Radious(radious), m_SliceCount(slicecount), m_StackCount(stackcount), m_Color(color)
{
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();
}

Sphere::~Sphere()
{
	Release();
}

void Sphere::Initialize()
{
	float pi_step = XM_PI / m_StackCount;
	float theta_step = 2.0f * XM_PI / m_SliceCount;
	Vertex vertex;
	vertex.pos.x = 0;
	vertex.pos.y = m_Radious;
	vertex.pos.z = 0;
	vertex.Color = m_Color;
	m_Sphere.push_back(vertex);

	for (int i = 1; i <= m_StackCount - 1 ; i++)
	{
		float pitmp = i * pi_step;

		for (int j = 0; j <= m_SliceCount; j++)
		{
			float thetatmp = j * theta_step;
			vertex.pos.x = m_Radious * sin(pitmp) * cos(thetatmp);
			vertex.pos.y = m_Radious * cos(pitmp);
			vertex.pos.z = m_Radious * sin(pitmp) * sin(thetatmp);
			vertex.Color = m_Color;
			m_Sphere.push_back(vertex);
		}
	}

	for (int i = 1; i <= m_SliceCount; i++) 
	{
		m_index.push_back(0);
		m_index.push_back(i + 1);
		m_index.push_back(i);
	}

	int baseIndex = 1;
	int ringVertexCount = m_SliceCount + 1;
	for (int i = 0; i < m_StackCount - 2; i++) 
	{
		for (int j = 0; j < m_SliceCount; j++) 
		{
			m_index.push_back(baseIndex + i * ringVertexCount + j);
			m_index.push_back(baseIndex + i * ringVertexCount + j + 1);
			m_index.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			
			m_index.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			m_index.push_back(baseIndex + i * ringVertexCount + j + 1);
			m_index.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	int southPoleIndex = m_Sphere.size() - 1;
	baseIndex = southPoleIndex - ringVertexCount;
	for (int i = 0; i <= m_SliceCount; i++) 
	{
		m_index.push_back(southPoleIndex);
		m_index.push_back(baseIndex + i);
		m_index.push_back(baseIndex + i + 1);
	}

	HRESULT res = S_OK;

	UINT size = sizeof(Vertex) * m_Sphere.size();

	res = D3DDevice::GetInstance()->CreateVB(m_Sphere.data(), size, &m_pVB);

	size = sizeof(int) * m_index.size();

	res = D3DDevice::GetInstance()->CreateIB(m_index.data(), size, &m_pIB);

	m_pShader = new Shader();

	m_pShader->ShaderLoad(L"../shader/Box.fx", m_pDevCopy);

	/// 각 셰이더마다 다르게 정점 레이아웃 설정 가능(모델 & 상황포함)
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION",	   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);
	m_pShader->CreateVBLayOut(layout, numElements, m_pDevCopy);

	ZeroMemory(&m_cbData, sizeof(CBGEOMTRY));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBGEOMTRY), &m_cbData, &m_pCbVertex, m_pDevCopy);

	// 셰이더 셋팅
	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);
}

void Sphere::Update(Matrix World)
{
	m_cbData.mView = D3DDevice::GetInstance()->m_View;
	m_cbData.mProj = D3DDevice::GetInstance()->m_Proj;
	m_cbData.mWorld = World;
}

void Sphere::ChangeColor(Vector4 col)
{
	for (int i = 0; i < m_Sphere.size(); i++)
	{
		m_Sphere[i].Color = col;
	}
}

void Sphere::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	m_pDevConCopy->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pDevConCopy->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, offset);
	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pCbVertex);
	m_pDevConCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDevConCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbVertex, &m_cbData, sizeof(CBGEOMTRY));

	//기본 셰이더 설정.
	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	m_pDevConCopy->DrawIndexed(m_index.size(), 0, 0);
}

void Sphere::Release()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pDevCopy);
	SAFE_RELEASE(m_pDevConCopy);
	SAFE_RELEASE(m_pCbVertex);
	m_pShader->ShaderRelease();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Line::Line(float startpos, float endpos, float thickness, Vector4 color)
{
	m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();

	m_Startpos = startpos;
	endpos = endpos;
	m_Thickness = thickness;
}

Line::~Line()
{

}

void Line::Initialize()
{
	HRESULT res = S_OK;

	UINT size = sizeof(Vertex) * m_Line.size();

	res = D3DDevice::GetInstance()->CreateVB(m_Line.data(), sizeof(size), &m_pVB);


	m_pShader = new Shader();

	m_pShader->ShaderLoad(L"../shader/Line.fx", m_pDevCopy);

	/// 각 셰이더마다 다르게 정점 레이아웃 설정 가능(모델 & 상황포함)
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "POSITION",	   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);
	m_pShader->CreateVBLayOut(layout, numElements, m_pDevCopy);

	ZeroMemory(&m_cbData, sizeof(CBGEOMTRY));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBGEOMTRY), &m_cbData, &m_pCbVertex, m_pDevCopy);

	// 셰이더 셋팅
	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);
}

void Line::Update(Matrix World)
{
	m_cbData.mView = D3DDevice::GetInstance()->m_View;
	m_cbData.mProj = D3DDevice::GetInstance()->m_Proj;
	m_cbData.mWorld = World;
}

void Line::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	m_pDevConCopy->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pDevConCopy->VSSetConstantBuffers(0, 1, &m_pCbVertex);
	m_pDevConCopy->IASetInputLayout(m_pShader->m_pVBLayout);
	m_pDevConCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	m_pShader->UpdateDynamicConstantBuffer(m_pDevConCopy, m_pCbVertex, &m_cbData, sizeof(CBGEOMTRY));

	//기본 셰이더 설정.
	m_pDevConCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDevConCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	m_pDevConCopy->Draw(m_Line.size(), 0);
}

void Line::Release()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pDevCopy);
	SAFE_RELEASE(m_pDevConCopy);
	SAFE_RELEASE(m_pCbVertex);
	m_pShader->ShaderRelease();
}
