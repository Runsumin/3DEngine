#include "pch.h"
#include "Model.h"

Model_default::Model_default()
{

}

Model_default::~Model_default()
{

}

void Model_default::InitModel(string filename, Matrix mat)
{
	//string filename = "..\\Resource\\knight\\Knight_AniTest\\Knight_Alive.FBX";
	//string filename = "..\\Resource\\knight\\Knight_Idle.FBX";
	//string filename = "..\\Resource\\Devil2\\Devil_Idle_Normal.FBX";
	//string filename = "..\\Map_Editor_Res\\Castle_Pillar_Type01\\Castle_Pillar_Type01.FBX";
	//string filename = "..\\Resource\\HealArea.FBX";
	//string filename = "..\\Map_Editor_Res\\House_03\\House_03.FBX";
	GRAPHICENGINE::CreateModelObject(filename);
	// Textrue
	GRAPHICENGINE::CreateModelMaterial_Texture();
	GRAPHICENGINE::SetModelObject(&m_pModel, filename);

	m_pModel->InitFbxModel();
	m_pModel->SetAnimationLoop(true);
	m_pModel->SetAniOn();


	m_pModel->SetUserMatrix(mat);
}

void Model_default::InitMaterial(int type)
{
	if (type == 0)
	{
		//MaskMap
		string fur = "..\\Resource\\Devil2\\fur_Mask_alpha.png";
		string wing = "..\\Resource\\Devil2\\wing_Mask_alpha.png";

		string ormbody = "..\\Resource\\Devil2\\devil_wingwing_body_2_OcclusionRoughnessMetallic.png";
		string ormbody_2 = "..\\Resource\\Devil2\\low_body_OcclusionRoughnessMetallic.png";
		string ormwing = "..\\Resource\\Devil2\\devil_wingwing_wing_OcclusionRoughnessMetallic.png";
		string ormprop = "..\\Resource\\Devil2\\devil_wingwing_prop_OcclusionRoughnessMetallic.png";
		string ormskull = "..\\Resource\\Devil2\\devil_wingwing_skull_OcclusionRoughnessMetallic.png";
		string ormprop001 = "..\\Resource\\Devil2\\devil_wingwing_skull_OcclusionRoughnessMetallic.png";
		string ormfur = "..\\Resource\\Devil2\\devil_wingwing_fur_OcclusionRoughnessMetallic.png";

		// ORM (Occlusion , Roughness, Metallic)
		int nodecount = m_pModel->GetModelNodeCount();
		for (int i = 0; i < nodecount; i++)
		{
			//임시 머테리얼 생성
			GRAPHICENGINE::IMaterialInterface* pmat;
			GRAPHICENGINE::CreateMaterial_Mesh(&pmat);
			// 노드 이름 받고
			string nodename = m_pModel->GetNodeName(i);
			switch (i)
			{
			case 0:
				pmat->SetTexturePath(ormwing, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				pmat->SetTexturePath(wing, GRAPHICENGINE::IMaterialInterface::TextureSort::MASKMAP);
				break;
			case 1:
				pmat->SetTexturePath(ormbody, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				break;
			case 2:
				pmat->SetTexturePath(ormbody_2, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				break;
			case 3:
				pmat->SetTexturePath(ormprop, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				break;
			case 4:
				pmat->SetTexturePath(ormskull, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				break;
			case 5:
				pmat->SetTexturePath(ormprop, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				break;
			case 6:
				pmat->SetTexturePath(ormfur, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				pmat->SetTexturePath(fur, GRAPHICENGINE::IMaterialInterface::TextureSort::MASKMAP);
				break;
			}
			pmat->SetLinkNodename(nodename);
			m_pDevilORMList.push_back(pmat);
		}

		int matcount = m_pDevilORMList.size();
		for (int i = 0; i < matcount; i++)
		{
			m_pModel->SetMaterialData(m_pDevilORMList[i], m_pDevilORMList[i]->GetLinkNodename(), GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
			m_pModel->SetMaterialData(m_pDevilORMList[i], m_pDevilORMList[i]->GetLinkNodename(), GRAPHICENGINE::IMaterialInterface::TextureSort::MASKMAP);
		}


		// FINAL
		m_pModel->InitModelTexture();
	}
	else if (type == 1)
	{
		// Base Color
		string arm = "..\\Resource\\knight\\black_arm_BaseColor.png";
		string bottom = "..\\Resource\\knight\\black_bottom_BaseColor.png";
		string helemat = "..\\Resource\\knight\\black_helmet_BaseColor.png";
		string shoes = "..\\Resource\\knight\\black_shoes_BaseColor.png";
		string top = "..\\Resource\\knight\\black_top_BaseColor.png";

		// Normal
		string armnor = "..\\Resource\\knight\\black_arm_Normal.png";
		string bottomnor = "..\\Resource\\knight\\black_bottom_Normal.png";
		string helematnor = "..\\Resource\\knight\\black_helmet_Normal.png";
		string shoesnor = "..\\Resource\\knight\\black_shoes_Normal.png";
		string topnor = "..\\Resource\\knight\\black_top_Normal.png";

		// AORM
		string armaorm = "..\\Resource\\knight\\black_arm_OcclusionRoughnessMetallic.png";
		string bottomaorm = "..\\Resource\\knight\\black_bottom_OcclusionRoughnessMetallic.png";
		string helemataorm = "..\\Resource\\knight\\black_helmet_OcclusionRoughnessMetallic.png";
		string shoesaorm = "..\\Resource\\knight\\black_shoes_OcclusionRoughnessMetallic.png";
		string topaorm = "..\\Resource\\knight\\black_top_OcclusionRoughnessMetallic.png";

		// 재질 정보 생성
		int nodecount = m_pModel->GetModelNodeCount();
		for (int i = 0; i < nodecount; i++)
		{
			// 임시 머테리얼 생성
			GRAPHICENGINE::IMaterialInterface* pmat;
			GRAPHICENGINE::CreateMaterial_Mesh(&pmat);
			// 노드 이름 받고
			string nodename = m_pModel->GetNodeName(i);
			switch (i)
			{
			case 0:
				pmat->SetTexturePath(arm, GRAPHICENGINE::IMaterialInterface::TextureSort::ALBEDO);
				pmat->SetTexturePath(armnor, GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
				pmat->SetTexturePath(armaorm, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				break;
			case 1:
				pmat->SetTexturePath(bottom, GRAPHICENGINE::IMaterialInterface::TextureSort::ALBEDO);
				pmat->SetTexturePath(bottomnor, GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
				pmat->SetTexturePath(bottomaorm, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				break;
			case 2:
				pmat->SetTexturePath(helemat, GRAPHICENGINE::IMaterialInterface::TextureSort::ALBEDO);
				pmat->SetTexturePath(helematnor, GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
				pmat->SetTexturePath(helemataorm, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				break;
			case 3:
				pmat->SetTexturePath(shoes, GRAPHICENGINE::IMaterialInterface::TextureSort::ALBEDO);
				pmat->SetTexturePath(shoesnor, GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
				pmat->SetTexturePath(shoesaorm, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				break;
			case 4:
				pmat->SetTexturePath(top, GRAPHICENGINE::IMaterialInterface::TextureSort::ALBEDO);
				pmat->SetTexturePath(topnor, GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
				pmat->SetTexturePath(topaorm, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				break;
			}
			pmat->SetLinkNodename(nodename);
			m_pBlackList.push_back(pmat);
		}

		int matcount = m_pBlackList.size();
		for (int i = 0; i < matcount; i++)
		{
			m_pModel->SetMaterialData(m_pBlackList[i], m_pBlackList[i]->GetLinkNodename(), GRAPHICENGINE::IMaterialInterface::TextureSort::ALBEDO);
			m_pModel->SetMaterialData(m_pBlackList[i], m_pBlackList[i]->GetLinkNodename(), GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
			m_pModel->SetMaterialData(m_pBlackList[i], m_pBlackList[i]->GetLinkNodename(), GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
		}

		m_pModel->InitModelTexture();
	}
	else if (type == 2)
	{
		// House1

		// Normal
		string WoodtrimNormal = "..\\Map_Editor_Res\\House_01\\T_Wood_Trim_Normal.png";
		string StoneNormal = "..\\Map_Editor_Res\\House_01\\T_Stone_Normal.png";
		string BrickNormal = "..\\Map_Editor_Res\\House_01\\T_Brick_Normal.png";
		string WoodBaseNormal = "..\\Map_Editor_Res\\House_01\\T_Wood_Normal.png";
		string PlasterNormal = "..\\Map_Editor_Res\\House_01\\T_Plaster_Normal.png";
		string RoofNormal = "..\\Map_Editor_Res\\House_01\\T_Roof_Normal.png";

		// AORM
		string Woodtrimorm = "..\\Map_Editor_Res\\House_01\\T_Wood_Trim_OcclusionRoughnessMetallic.png";
		string Stoneaorm = "..\\Map_Editor_Res\\House_01\\T_Stone_OcclusionRoughnessMetallic.png";
		string Brickaorm = "..\\Map_Editor_Res\\House_01\\T_Brick_OcclusionRoughnessMetallic.png";
		string WoodBaseaorm = "..\\Map_Editor_Res\\House_01\\T_Wood_OcclusionRoughnessMetallic.png";
		string Plasteraorm = "..\\Map_Editor_Res\\House_01\\T_Plaster_OcclusionRoughnessMetallic.png";
		string Roofaorm = "..\\Map_Editor_Res\\House_01\\T_Roof_OcclusionRoughnessMetallic.png";

		for (int i = 0; i < 6; i++)
		{
			// 임시 머테리얼 생성
			GRAPHICENGINE::IMaterialInterface* pmat;
			string name;
			GRAPHICENGINE::CreateMaterial_Mesh(&pmat);

			switch (i)
			{
			case 0:
				pmat->SetTexturePath(WoodtrimNormal, GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
				pmat->SetTexturePath(Woodtrimorm, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				name = "1_Building_01";
				break;
			case 1:
				pmat->SetTexturePath(StoneNormal, GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
				pmat->SetTexturePath(Stoneaorm, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				name = "1_Building_03";
				break;
			case 2:
				pmat->SetTexturePath(BrickNormal, GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
				pmat->SetTexturePath(Brickaorm, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				name = "1_Building_07";
				break;
			case 3:
				pmat->SetTexturePath(WoodBaseNormal, GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
				pmat->SetTexturePath(WoodBaseaorm, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				name = "1_Building_31";
				break;
			case 4:
				pmat->SetTexturePath(PlasterNormal, GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
				pmat->SetTexturePath(Plasteraorm, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				name = "1_Building_59";
				break;
			case 5:
				pmat->SetTexturePath(RoofNormal, GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
				pmat->SetTexturePath(Roofaorm, GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
				name = "1_Building_73";
				break;
			}
			pmat->SetLinkNodename(name);
			m_pHouseMat.push_back(pmat);
		}

		int matcount = m_pHouseMat.size();
		for (int i = 0; i < matcount; i++)
		{
			m_pModel->SetMaterialData(m_pHouseMat[i], m_pHouseMat[i]->GetLinkNodename(), GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
			m_pModel->SetMaterialData(m_pHouseMat[i], m_pHouseMat[i]->GetLinkNodename(), GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
		}

		m_pModel->InitModelTexture();
	}
	else if (type == 3)
	{
		// 석판 Emissive
		string EmissiveMap = "..\\Map_Editor_Res\\Slate\\T_Obelisk_Stone_01_Blue_Emissive.png";
		string name = "SM_Obelisk_Stone";
		// 임시 머테리얼 생성
		GRAPHICENGINE::IMaterialInterface* pmat;
		GRAPHICENGINE::CreateMaterial_Mesh(&pmat);
		pmat->SetTexturePath(EmissiveMap, GRAPHICENGINE::IMaterialInterface::TextureSort::EMISSIVE);
		pmat->SetLinkNodename(name);
		m_pHouseMat.push_back(pmat);

		m_pModel->SetMaterialData(pmat, pmat->GetLinkNodename(), GRAPHICENGINE::IMaterialInterface::TextureSort::EMISSIVE);
		m_pModel->InitModelTexture();
		m_pModel->ChangeApplyEmissiveMap(3);

		m_pModel->SetEmissiveMapOnOff(true);

		//string AddEmissiveMap = "..\\Map_Editor_Res\\Slate\\T_Obelisk_Stone_02_Red_Emissive.png";
		//m_pModel->AddEmissiveMap(AddEmissiveMap);
	}
	else if (type == 4) 
	{
		// 석판 Emissive
		string EmissiveMap = "..\\Map_Editor_Res\\Obelisk\\T_Obelisk_Floor_Arrow_01_Blue_Emissive.png";
		string name = "SM_Obelisk_Floor_02";
		// 임시 머테리얼 생성
		GRAPHICENGINE::IMaterialInterface* pmat;
		GRAPHICENGINE::CreateMaterial_Mesh(&pmat);
		pmat->SetTexturePath(EmissiveMap, GRAPHICENGINE::IMaterialInterface::TextureSort::EMISSIVE);
		pmat->SetLinkNodename(name);
		m_pHouseMat.push_back(pmat);

		m_pModel->SetMaterialData(pmat, pmat->GetLinkNodename(), GRAPHICENGINE::IMaterialInterface::TextureSort::EMISSIVE);
		m_pModel->InitModelTexture();
		m_pModel->ChangeApplyEmissiveMap(3);

		m_pModel->SetEmissiveMapOnOff(true);

	}
	else if (type == 5)
	{
		// 석판 Emissive
		string EmissiveMap = "..\\Map_Editor_Res\\Obelisk\\T_Obelisk_Floor_Arrow_01_Blue_Emissive.png";
		string name = "SM_Obelisk_Floor_02";
		// 임시 머테리얼 생성
		GRAPHICENGINE::IMaterialInterface* pmat;
		GRAPHICENGINE::CreateMaterial_Mesh(&pmat);
		pmat->SetTexturePath(EmissiveMap, GRAPHICENGINE::IMaterialInterface::TextureSort::EMISSIVE);
		pmat->SetLinkNodename(name);
		m_pHouseMat.push_back(pmat);

		m_pModel->SetMaterialData(pmat, pmat->GetLinkNodename(), GRAPHICENGINE::IMaterialInterface::TextureSort::EMISSIVE);
		m_pModel->InitModelTexture();

		string AddEmissiveMap = "..\\Map_Editor_Res\\Obelisk\\T_Obelisk_Floor_Arrow_01_Red_Emissive.png";
		m_pModel->AddEmissiveMap(AddEmissiveMap);

		m_pModel->ChangeApplyEmissiveMap(2);
		m_pModel->SetEmissiveMapOnOff(true);

	}

	else if (type == 6)
	{
	// 석판 Emissive
	string EmissiveMap = "..\\Map_Editor_Res\\Obelisk\\T_Obelisk_Floor_Arrow_01_Blue_Emissive.png";
	string name = "SM_Obelisk_Floor_02";
	// 임시 머테리얼 생성
	GRAPHICENGINE::IMaterialInterface* pmat;
	GRAPHICENGINE::CreateMaterial_Mesh(&pmat);
	pmat->SetTexturePath(EmissiveMap, GRAPHICENGINE::IMaterialInterface::TextureSort::EMISSIVE);
	pmat->SetLinkNodename(name);
	m_pHouseMat.push_back(pmat);

	m_pModel->SetMaterialData(pmat, pmat->GetLinkNodename(), GRAPHICENGINE::IMaterialInterface::TextureSort::EMISSIVE);
	m_pModel->InitModelTexture();

	string AddEmissiveMap = "..\\Map_Editor_Res\\Obelisk\\T_Obelisk_Floor_Arrow_01_Red_Emissive.png";
	m_pModel->AddEmissiveMap(AddEmissiveMap);

	m_pModel->ChangeApplyEmissiveMap(3);
	m_pModel->ChangeApplyEmissiveMap(2);
	m_pModel->SetEmissiveMapOnOff(false);

	}
}

void Model_default::Update(float enignetime)
{
	m_pModel->FbxModelUpdate(enignetime);

	//if (Input::Ins()->GetKeyDown(Keyboard::Z))
	//{
	//	m_pModel->ChangeApplyEmissiveMap(1);
	//}
	//if (Input::Ins()->GetKeyDown(Keyboard::X))
	//{
	//	m_pModel->ChangeApplyEmissiveMap(0);
	//}

	//if (GetAsyncKeyState(VK_F8) & 0x8001 == 0x8001)
	//{
	//	m_b ^= true;
	//}

	//float data = XM_PIDIV2 * enignetime;

	//if (m_b == true)
	//{
	//	m_Angle += data;
	//	Matrix spin = Matrix::CreateRotationY(m_Angle);
	//	Matrix scale = XMMatrixScaling(1.1f, 1.1f, 1.1f);
	//	Matrix 	trans = Matrix::CreateTranslation(Vector3(20.0f, 2.45f, 0.0f));
	//	m_pModel->SetUserMatrix(scale * spin * trans);
	//}
}

void Model_default::ChangeMaterial()
{

}

Matrix Model_default::GetMarbyname(string name)
{
	return m_pModel->GetHiearchyMatrixByName(name);
}

void Model_default::SetMatrix(Matrix mat)
{
	m_pModel->SetUserMatrix(mat);
}
