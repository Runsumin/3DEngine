#include "pch.h"
#include "Player.h"
#include "Input.h"

Player::Player()
	:m_PlayerState(state::IDLE), m_LatePlayerState(state::IDLE), m_Idle(0), m_Walk(0), m_Run(0), m_Attack(0),
	m_Position(Vector3::Zero), m_Rotation(Vector3::Zero), m_Scale(Vector3::One), m_Dir(Vector3(0, 0, -1)), m_Velocity(10.f),
	m_bOnce(false), m_bRun(false), m_b(false), m_Angle(0.f)
{

}
Player::~Player()
{

}

void Player::InitPlayer(HWND hwnd)
{
	cout << "Player Create..." << endl;
	string devil = "..\\Resource\\Devil2\\Devil_Idle_Normal.FBX";
	GRAPHICENGINE::CreateModelObject(devil);
	GRAPHICENGINE::CreateModelMaterial_Texture();
	GRAPHICENGINE::SetModelObject(&m_pModel, devil);

	Input::Ins()->Initalize(hwnd);

	Matrix trans, scl;
	trans = Matrix::CreateTranslation(Vector3(0.0f, 10.0f, 0.0f));
	scl = Matrix::CreateScale(10.01f, 10.01f, 10.01f);

	m_pModel->SetUserMatrix(scl * trans);
	m_pModel->InitFbxModel();
	m_pModel->SetAnimationLoop(true);
	m_pModel->SetAniOn();
	m_pModel->SetAnimationSpeed(1.0f);
}

void Player::SetMaterial()
{
	//MaskMap
	string fur = "..\\Resource\\Devil2\\BPR_Mask_alpha.png";
	string wing = "..\\Resource\\Devil2\\BPR_Mask_1_alpha.png";

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

void Player::Update(float time)
{
	m_pModel->FbxModelUpdate(time);

	if (GetAsyncKeyState(VK_F8) & 0x8001 == 0x8001)
	{
		m_b ^= true;
	}

	float data = XM_PIDIV2 * time;

	if (m_b == true)
	{
		m_Angle += data;
		Matrix spin = Matrix::CreateRotationY(m_Angle);
		Matrix scale = XMMatrixScaling(10.1f, 10.1f, 10.1f);
		Matrix 	trans = Matrix::CreateTranslation(Vector3(0.0f, 10.0f, 0.0f));
		m_pModel->SetUserMatrix(scale * spin * trans);
	}

	//KeyInput();
	//PlayerAniChage();
	//PlayerTransFormChange();

	//m_LatePlayerState = m_PlayerState;
}

void Player::KeyInput()
{
	Input::Ins()->Update();
	m_Dir = Vector3(0, 0, -1);

	if (m_Idle + m_Walk + m_Run + m_Attack == 0
		&& m_bRun == false)
	{
		m_PlayerState = state::IDLE;
	}

	m_Idle = 0;
	m_Walk = 0;
	m_Run = 0;
	m_Attack = 0;

	//if (Input::Ins()->GetKeyDown(Keyboard::LeftShift))
	//{
	//	m_Run = 1;
	//	m_bRun = true;
	//}
	//if (Input::Ins()->GetKeyUp(Keyboard::LeftShift) && m_bRun == true)
	//{
	//	m_bRun = false;
	//}
	if (Input::Ins()->GetKeyDown(Keyboard::U) ||
		Input::Ins()->GetKeyDown(Keyboard::J)||
		Input::Ins()->GetKeyDown(Keyboard::H)||
		Input::Ins()->GetKeyDown(Keyboard::K))
	{
		m_Walk = 1;
		m_PlayerState = state::WALK;

		if (m_bRun == true)
		{
			m_PlayerState = state::RUN;
		}

	}

	if (Input::Ins()->GetMouseButtonDown(Input::eMouse::leftButton))
	{
		m_PlayerState = state::ATTACK;
	}

	//switch (m_PlayerState)
	//{
	//case Player::state::IDLE:
	//	cout << "1" << endl;
	//	break;
	//case Player::state::WALK:
	//	cout << "2" << endl;
	//	break;
	//case Player::state::RUN:
	//	cout << "3" << endl;
	//	break;
	//case Player::state::ATTACK:
	//	cout << "4" << endl;
	//	break;
	//}
}

void Player::PlayerAniChage()
{
	if (m_LatePlayerState != m_PlayerState)
	{
		switch (m_PlayerState)
		{
		case Player::state::IDLE:
			m_pModel->ChangeAnimation(0);
			break;
		case Player::state::WALK:
			m_pModel->ChangeAnimation(1);
			break;
		case Player::state::RUN:
			m_pModel->ChangeAnimation(5);
			break;
		case Player::state::ATTACK:
			m_pModel->ChangeAnimation(2);
			break;
		}

	}
}

void Player::PlayerTransFormChange()
{
	switch (m_PlayerState)
	{
	case Player::state::IDLE:
		break;
	case Player::state::WALK:
		break;
	case Player::state::RUN:
		break;
	case Player::state::ATTACK:
		break;
	}
}
