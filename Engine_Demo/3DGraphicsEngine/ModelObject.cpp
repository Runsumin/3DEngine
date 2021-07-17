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
	// ���� �����͸� �д� �κ�..
	//�ε��� ase ���ϸ��� �ܺ����Ͽ��� �о��. 
	//�̶�  ���ϸ��� 'path' �� ���Եȴ�.  
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

	/// �� ���̴����� �ٸ��� ���� ���̾ƿ� ���� ����(�� & ��Ȳ����)
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

	// �⺻ ���� ���� ������� ����
	ZeroMemory(&m_CbDef, sizeof(CBDEFAULT));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBDEFAULT), &m_CbDef, &m_pCBDef, m_pDeviceCopy);
	ZeroMemory(&m_CbMtl, sizeof(CBMATERIAL));
	m_pShader->CreateDynamicConstantBuffer(sizeof(CBMATERIAL), &m_CbMtl, &m_pCBMtl, m_pDeviceCopy);

	// ���̴� ����
	m_pDeviceCtCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
	m_pDeviceCtCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

	m_pDeviceCtCopy->VSSetConstantBuffers(0, 1, &m_pCBDef);
	m_pDeviceCtCopy->VSSetConstantBuffers(1, 1, &m_pCBMtl);






	return OK_SIGN;
}

bool ModelObject::ModelUpdate(float dTime)
{
	///////
	// ����TM
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





	// �� ���, ������� ����
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

		// �ؽ��� ������ �ε�.. �ϴ� ���� �����ϱ� �߰�
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

		//���� ��� ����.
		m_CbDef.mWV = m_CbDef.mTm * m_CbDef.mView;
		m_CbDef.mWVP = m_CbDef.mTm * m_CbDef.mView * m_CbDef.mProj;

		m_pShader->UpdateDynamicConstantBuffer(D3DDevice::GetInstance()->GetDeviceContext(), m_pCBDef, &m_CbDef, sizeof(CBDEFAULT));

		//�⺻ ���̴� ����.
		m_pDeviceCtCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
		m_pDeviceCtCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

		//�⺻ ���̴� ������� ����.
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

		// �ؽ��� ������ �ε�.. �ϴ� ���� �����ϱ� �߰�
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

		//���� ��� ����.
		m_CbDef.mWV = m_CbDef.mTm * m_CbDef.mView;
		m_CbDef.mWVP = m_CbDef.mTm * m_CbDef.mView * m_CbDef.mProj;

		m_pShader->UpdateDynamicConstantBuffer(D3DDevice::GetInstance()->GetDeviceContext(), m_pCBDef, &m_CbDef, sizeof(CBDEFAULT));

		//�⺻ ���̴� ����.
		m_pDeviceCtCopy->VSSetShader(m_pShader->m_pVS, nullptr, 0);
		m_pDeviceCtCopy->PSSetShader(m_pShader->m_pPS, nullptr, 0);

		//�⺻ ���̴� ������� ����.
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
