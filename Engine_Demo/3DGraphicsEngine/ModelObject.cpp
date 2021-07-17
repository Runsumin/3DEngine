#include "ModelObject.h"

ModelObject::ModelObject()
	:m_pObj(NULL), m_pShader(NULL), m_pCBDef(nullptr), m_pCBMtl(nullptr)
	, m_pDeviceCopy(nullptr), m_pDeviceCtCopy(nullptr)
{
	ZeroMemory(&m_CbDef, sizeof(CBDEFAULT));
	ZeroMemory(&m_CbMtl, sizeof(CBMATERIAL));

	m_UserTm = XMMatrixIdentity();
	m_Transm = m_Rotm = m_Scalem = m_UserTm;

	m_vPos = Vector3::Zero;
	m_vRot = Vector3::Zero;
	m_vScale = Vector3::Zero;
}

ModelObject::~ModelObject()
{

}

int ModelObject::LoadModelFromFile(TCHAR* filename, ID3D11Device* pdevice, ID3D11DeviceContext* pDeviceContext)
{
	// 모델의 데이터를 읽는 부분..
	//로드할 ase 파일명을 외부파일에서 읽어옴. 
	//이때  파일명은 'path' 가 포함된다.  
	//TCHAR filename[256] = L"";
	//ID3D11Device* pdevice = D3DDevice::GetInstance()->GetDevice();

	if (ModelCreateFromASE(pdevice, filename, &m_pObj, pDeviceContext) == FAIL_SIGN)
	{
		return FAIL_SIGN;
	}

	m_pObj->initializeModel();
	return OK_SIGN;
}

int ModelObject::initModel()
{
	m_pDeviceCopy = D3DDevice::GetInstance()->GetDevice();
	m_pDeviceCtCopy = D3DDevice::GetInstance()->GetDeviceContext();
	return OK_SIGN;
}

int ModelObject::initShader()
{
	m_pShader = new Shader();

	m_pShader->ShaderLoad(L"./shader/Default.fx", m_pDeviceCopy);

	/// 각 셰이더마다 다르게 정점 레이아웃 설정 가능(모델 & 상황포함)
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                    offset         classification             
		{ "BLENDWEIGHT",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",  0, DXGI_FORMAT_R32G32B32A32_UINT,  0,  16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION",	   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",        0, DXGI_FORMAT_R32G32B32_FLOAT,	  0,  44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",      0, DXGI_FORMAT_R32G32_FLOAT,		  0,  56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	m_pShader->CreateVBLayOut(layout, numElements, m_pDeviceCopy);

	// 기본 정부 운용용 상수버퍼 생성
	ZeroMemory(&m_CbDef, sizeof(CBDEFAULT));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBDEFAULT), &m_CbDef, &m_pCBDef, m_pDeviceCopy);
	ZeroMemory(&m_CbMtl, sizeof(CBMATERIAL));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBMATERIAL), &m_CbMtl, &m_pCBMtl, m_pDeviceCopy);

	// 셰이더 셋팅
	m_pDeviceCtCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDeviceCtCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	m_pDeviceCtCopy->VSSetConstantBuffers(0, 1, &m_pCBDef);
	m_pDeviceCtCopy->VSSetConstantBuffers(1, 1, &m_pCBMtl);






	return OK_SIGN;
}

bool ModelObject::ModelUpdate(float dTime)
{
	///////
	// 유저TM
	//m_vScale = Vector3(20.f, 20.f, 20.f);
	m_vScale = Vector3(1.0f, 1.0f, 1.0f);


	m_Scalem = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
	m_Rotm = XMMatrixRotationRollPitchYaw(m_vRot.x, m_vRot.y, m_vRot.z);
	m_Transm = XMMatrixTranslation(m_vScale.x, m_vScale.y, m_vScale.z);

	m_UserTm = m_Scalem * m_Rotm * m_Transm;

	m_pObj->AseModelUpdate(dTime);

	int cnt = m_pObj->m_BoneList.size();

	for (int i = 0; i < cnt; i++)
	{
		XMMATRIX _Temp = XMMatrixIdentity();
		_Temp = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_pObj->m_BoneList[i]->m_mTM));
		m_CbDef.m_BoneTm[i] = _Temp * m_pObj->m_BoneList[i]->GetFinalTM();
	}





	// 뷰 행렬, 투영행렬 전달
	Camera::GetInstance()->SetCameraMatrixData(&m_CbDef.mView, &m_CbDef.mProj);

	return true;
}

bool ModelObject::ModelDraw(float dTime)
{
	//m_pObj->AseModelDraw();

	int cnt = m_pObj->m_NodeList.size();
	for (int i = 0; i < cnt; i++)
	{
		CBMATERIAL Temp; 
		ZeroMemory(&Temp, sizeof(CBMATERIAL));
		Temp.mDiffuse = m_pObj->m_ModelMtlList[m_pObj->m_NodeList[i]->GetMtrlCount()].m_Diffuse;
		Temp.mAmbient = m_pObj->m_ModelMtlList[m_pObj->m_NodeList[i]->GetMtrlCount()].m_Ambient;
		Temp.mSpecular = m_pObj->m_ModelMtlList[m_pObj->m_NodeList[i]->GetMtrlCount()].m_Specular;
		Temp.mSpecPower = m_pObj->m_ModelMtlList[m_pObj->m_NodeList[i]->GetMtrlCount()].m_SpcePower;

		m_pShader->UpdateDynamicConstantBuffer(D3DDevice::GetInstance()->GetDeviceContext(), m_pCBMtl, &Temp, sizeof(CBMATERIAL));
		m_pDeviceCtCopy->VSSetConstantBuffers(1, 1, &m_pCBMtl);

		// 텍스쳐 있으면 로드.. 일단 곰은 있으니깐 추가
		m_pDeviceCtCopy->PSSetShaderResources(0, 1, &m_pObj->m_ModelMtlList[m_pObj->m_NodeList[i]->GetMtrlCount()].m_pTexture);
		m_pDeviceCtCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_CLAMP]);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////

		UINT stride = sizeof(VERTEX_ASE_EXTEND);
		UINT offset = 0;

		m_pDeviceCtCopy->IASetVertexBuffers(0, 1, &m_pObj->m_NodeList[i]->m_pVB, &stride, &offset);
		m_pDeviceCtCopy->IASetIndexBuffer(m_pObj->m_NodeList[i]->m_pIB, DXGI_FORMAT_R32_UINT, offset);

		m_pDeviceCtCopy->IASetInputLayout(m_pShader->m_pVBLayout);
		m_pDeviceCtCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		XMMATRIX mTm = XMMatrixIdentity();

		mTm = m_pObj->m_NodeList[i]->GetFinalTM() * m_UserTm;

		m_CbDef.mTm = mTm;

		//최종 행렬 결합.
		m_CbDef.mWV = m_CbDef.mTm * m_CbDef.mView;
		m_CbDef.mWVP = m_CbDef.mTm * m_CbDef.mView * m_CbDef.mProj;

		m_pShader->UpdateDynamicConstantBuffer(D3DDevice::GetInstance()->GetDeviceContext(), m_pCBDef, &m_CbDef, sizeof(CBDEFAULT));

		//기본 셰이더 설정.
		m_pDeviceCtCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
		m_pDeviceCtCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

		//기본 셰이더 상수버퍼 설정.
		m_pDeviceCtCopy->VSSetConstantBuffers(0, 1, &m_pCBDef);
		m_pDeviceCtCopy->VSSetConstantBuffers(1, 1, &m_pCBMtl);

		m_pDeviceCtCopy->DrawIndexed(m_pObj->m_NodeList[i]->m_IndexCnt, 0, 0);

	}

	int cnt2 = m_pObj->m_BoneList.size();
	for (int i = 0; i < cnt2; i++)
	{
		CBMATERIAL Temp;
		ZeroMemory(&Temp, sizeof(CBMATERIAL));
		Temp.mDiffuse = m_pObj->m_ModelMtlList[m_pObj->m_BoneList[i]->GetMtrlCount()].m_Diffuse;
		Temp.mAmbient = m_pObj->m_ModelMtlList[m_pObj->m_BoneList[i]->GetMtrlCount()].m_Ambient;
		Temp.mSpecular = m_pObj->m_ModelMtlList[m_pObj->m_BoneList[i]->GetMtrlCount()].m_Specular;
		Temp.mSpecPower = m_pObj->m_ModelMtlList[m_pObj->m_BoneList[i]->GetMtrlCount()].m_SpcePower;

		m_pShader->UpdateDynamicConstantBuffer(D3DDevice::GetInstance()->GetDeviceContext(), m_pCBMtl, &Temp, sizeof(CBMATERIAL));
		m_pDeviceCtCopy->VSSetConstantBuffers(1, 1, &m_pCBMtl);

		// 텍스쳐 있으면 로드.. 일단 곰은 있으니깐 추가
		//m_pDeviceCtCopy->PSSetShaderResources(0, 1, &m_pObj->m_ModelMtlList[m_pObj->m_BoneList[i]->GetMtrlCount()].m_pTexture);
		//m_pDeviceCtCopy->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_CLAMP]);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////

		UINT stride = sizeof(VERTEX_ASE_EXTEND);
		UINT offset = 0;

		m_pDeviceCtCopy->IASetVertexBuffers(0, 1, &m_pObj->m_BoneList[i]->m_pVB, &stride, &offset);
		m_pDeviceCtCopy->IASetIndexBuffer(m_pObj->m_BoneList[i]->m_pIB, DXGI_FORMAT_R32_UINT, offset);

		m_pDeviceCtCopy->IASetInputLayout(m_pShader->m_pVBLayout);
		m_pDeviceCtCopy->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		XMMATRIX mTm = XMMatrixIdentity();

		mTm = m_pObj->m_BoneList[i]->GetFinalTM();

		m_CbDef.mTm = mTm;

		//최종 행렬 결합.
		m_CbDef.mWV = m_CbDef.mTm * m_CbDef.mView;
		m_CbDef.mWVP = m_CbDef.mTm * m_CbDef.mView * m_CbDef.mProj;

		m_pShader->UpdateDynamicConstantBuffer(D3DDevice::GetInstance()->GetDeviceContext(), m_pCBDef, &m_CbDef, sizeof(CBDEFAULT));

		//기본 셰이더 설정.
		m_pDeviceCtCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
		m_pDeviceCtCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

		//기본 셰이더 상수버퍼 설정.
		m_pDeviceCtCopy->VSSetConstantBuffers(0, 1, &m_pCBDef);
		m_pDeviceCtCopy->VSSetConstantBuffers(1, 1, &m_pCBMtl);

		m_pDeviceCtCopy->DrawIndexed(m_pObj->m_BoneList[i]->m_IndexCnt, 0, 0);

	}
	return true;
}

void ModelObject::ModelRelease()
{
	m_pObj->AseModelRelease();
}
