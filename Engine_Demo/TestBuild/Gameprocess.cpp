#include "pch.h"
#include "Gameprocess.h"

Gameprocess::Gameprocess()
{

}

Gameprocess::~Gameprocess()
{

}

HRESULT Gameprocess::Initialize(HINSTANCE hinstance)
{
	wchar_t AppName[] = L"Model Viewer";

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Gameprocess::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hinstance;
	wcex.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_TESTBUILD));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TESTBUILD);
	wcex.lpszClassName = AppName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	// 윈도우 생성
	m_hWnd = CreateWindowW(AppName, AppName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, m_ScreenWidth, m_ScreenHeight, nullptr, nullptr, hinstance, nullptr);

	// 컴객체 초기화
	if (FAILED(CoInitialize(NULL)))
	{
		return S_FALSE;
	}

	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	EngineSetting();

	HRESULT hr = S_OK;
	return hr;
}

void Gameprocess::EngineSetting()
{
	GRAPHICENGINE::EngineSetUp(m_hWnd);
	GRAPHICENGINE::FbxLoaderSetUp();
	m_BackBufferCol = Vector4(0.6, 0.6, 0.6, 1);

	Vector3 pos = Vector3(-12, 6, 29);
	Vector3 rot = Vector3(-0.03, 2.557, 0);

	/// Camera
	GRAPHICENGINE::CreateCamera(&m_pMainCamera, pos, rot);
	m_pMainCamera->InitializeCamera();
	m_pMainCamera->ChangeCameraMode(GRAPHICENGINE::ICameraInterface::Mode::MOVE);
	m_pMainCamera->SetCameraSpeed(30.f);

	/// Grid
	GRAPHICENGINE::CreateGird_Debug(&m_pGrid);
	m_pGrid->InitializeGrid(10, 10, 10);

	/// Skybox
	string skyboxfilename = "..\\Resource\\Skybox\\SkyBox3_1.dds";
	GRAPHICENGINE::CreateSkybox_Obj(&m_pSkybox, skyboxfilename);
	m_pSkybox->InitSkybox();
	EngineTime = 0;
	m_bWireFrame = false;
	m_AniChange = false;
	rot = Vector3(1, 1, 0);

	string uinname = "..\\Resource\\UI\\Dummy\\UI_Test.jpg";


	/// Terrain
	string filenameter = "../Resource/floor/MDP_Tex_PreB_4K_SquareStoneTilesWithMoss_AlbedoTransparency.tif";
	//string filenameter = "../Resource/Terrain_01.dds";
	GRAPHICENGINE::CreateTerrain_Obj(&m_pTerrain, filenameter, 200, 200, 20);
	m_pTerrain->InitTerrain();



	string Knight = "..\\Resource\\knight\\Knight_Idle.FBX";
	string devil = "..\\Resource\\Devil2\\Devil_Idle_Normal.FBX";
	string House = "..\\Map_Editor_Res\\House_02\\House_02.FBX";
	string Slate = "..\\Map_Editor_Res\\Slate\\Slate.FBX";
	string Obelisk = "..\\Map_Editor_Res\\Obelisk\\Obelisk.FBX";
	string Heal = "..\\Resource\\Heal\\Paladin_HealFX.FBX";

	Matrix scale;
	scale *= XMMatrixScaling(5.0f, 5.0f, 5.0f);

	DrawText_Debug::GetInstance()->CreateFontW();

	/// Shadow
	GRAPHICENGINE::SetShadow(true, Vector3(0, 0, 1), 70.f);
	//GRAPHICENGINE::SetShadow(true, Vector3(0, 0, 1), 40.f);
	/// Culling
	GRAPHICENGINE::SetViewFrustumCulling(false);

	///DirectionLight
	GRAPHICENGINE::CreateDirectionLight(&m_pDirLit);
	m_pDirLit->InitializeDirectrionLight(Vector4(1, -3, 2, 1), Vector4(1, 1, 1, 1), Vector4(0.2f, 0.2f, 0.2f, 1.0f));
	GRAPHICENGINE::GetDirectionLightData(Vector4(1, -3, 2, 1), Vector4(1, 1, 1, 1), Vector4(0.2f, 0.2f, 0.2f, 1.0f));

	GRAPHICENGINE::SetFogOnOff(true);

	/// Obelisk
	m_Obelisk = new Model_default();
	Matrix obelisktrans = Matrix::CreateTranslation(Vector3(20.0f, 12.0f, 13.0f));
	m_Obelisk->InitModel(Obelisk, scale * obelisktrans);
	m_Obelisk->InitMaterial(4);


	/// Slate
	m_Slate = new Model_default();
	Matrix slatetrans = Matrix::CreateTranslation(Vector3(-10.0f, 6.0f, 13.0f));
	Matrix slateRotation = XMMatrixRotationRollPitchYawFromVector(Vector3(0, -40, 0));
	m_Slate->InitModel(Slate, scale * slateRotation * slatetrans);
	m_Slate->InitMaterial(3);


	/// Knight
	m_pKnight = new Model_default();
	Matrix playertrans = Matrix::CreateTranslation(Vector3(0.0f, 4.0f, 20.0f));
	Matrix PlayerRotation = XMMatrixRotationRollPitchYawFromVector(Vector3(0, 230, 0));
	m_pKnight->InitModel(Knight, scale * PlayerRotation * playertrans);
	m_pKnight->InitMaterial(1);

	///// Knight
	//m_pHouse = new Model_default();
	//Matrix Housetrans = Matrix::CreateTranslation(Vector3(0.0f, 20.0f, -20.0f));
	//Matrix HouseRotation = XMMatrixRotationRollPitchYawFromVector(Vector3(0, 180, 0));
	//m_pHouse->InitModel(House, HouseRotation * Housetrans);
	//m_pHouse->InitMaterial(2);


	///Devil
	m_pPlayer = new Player();
	m_pPlayer->InitPlayer(m_hWnd);
	m_pPlayer->SetMaterial();

	/// Map
	//m_pMap = new Map();
	//m_pMap->InitializeMapData();

}

void Gameprocess::GameLoop()
{
	while (true)
	{
		if (PeekMessage(&m_Msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_Msg.message == WM_QUIT) break;

			DispatchMessage(&m_Msg);
		}
		else
		{
			EngineTime = GRAPHICENGINE::GetEngineTime();
			m_FPS = GRAPHICENGINE::GetFramePerSecond();
			UpdateAll();
			RenderAll();
		}
	}
}

void Gameprocess::UpdateAll()
{
	m_pTerrain->Update();
	m_Slate->Update(EngineTime);
	m_Obelisk->Update(EngineTime);
	m_pPlayer->Update(EngineTime);
	m_pKnight->Update(EngineTime);
	//m_pHouse->Update(EngineTime);
	//m_pMap->UpdatMap(EngineTime);

	//프로그램 종료 처리.
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		PostQuitMessage(WM_QUIT);
	}

	m_pMainCamera->UpdateCamera(EngineTime);
	GRAPHICENGINE::GetCameraMatrixOnce(m_pMainCamera->GetCameraViewMat(), m_pMainCamera->GetCameraProjMat());

	if (GetAsyncKeyState(NUM_1) & 0x8000)
	{
		GRAPHICENGINE::SetChannelView(GRAPHICENGINE::eChannelView::DEFAULT);
	}
	if (GetAsyncKeyState(NUM_2) & 0x8000)
	{
		GRAPHICENGINE::SetChannelView(GRAPHICENGINE::eChannelView::DIFFUSE);
	}
	if (GetAsyncKeyState(NUM_3) & 0x8000)
	{
		GRAPHICENGINE::SetChannelView(GRAPHICENGINE::eChannelView::NORMAL);
	}
	if (GetAsyncKeyState(NUM_4) & 0x8000)
	{
		GRAPHICENGINE::SetChannelView(GRAPHICENGINE::eChannelView::AO);
	}
	if (GetAsyncKeyState(NUM_5) & 0x8000)
	{
		GRAPHICENGINE::SetChannelView(GRAPHICENGINE::eChannelView::MATALIC);
	}
	if (GetAsyncKeyState(NUM_6) & 0x8000)
	{
		GRAPHICENGINE::SetChannelView(GRAPHICENGINE::eChannelView::ROUGHNESS);
	}
	if (GetAsyncKeyState(NUM_7) & 0x8000)
	{
		GRAPHICENGINE::SetChannelView(GRAPHICENGINE::eChannelView::IBL);
	}
	if (GetAsyncKeyState(NUM_8) & 0x8000)
	{
		GRAPHICENGINE::SetChannelView(GRAPHICENGINE::eChannelView::SHADOWMAP);
	}
	//if (GetAsyncKeyState(NUM_9) & 0x8000)
	//{
	//	GRAPHICENGINE::SetChannelView(GRAPHICENGINE::eChannelView::);
	//}


}

void Gameprocess::RenderAll()
{
	GRAPHICENGINE::BeginRender(m_BackBufferCol);


	GRAPHICENGINE::DrawAll(EngineTime);

	// DEBUGINFO
	DrawText_Debug::GetInstance()->DrawTextExW(0, 0, XMFLOAT4(255, 255, 255, 1), L"[FPS] : %f ", m_FPS);
	m_pMainCamera->CameraInfo();

	GRAPHICENGINE::EndRender();

	/// 렌더 모드 설정.... 
	if (m_bWireFrame == false)
	{
		GRAPHICENGINE::BloomOnOff(false);
	}
	else
	{
		GRAPHICENGINE::BloomOnOff(true);
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8001 == 0x8001)
	{
		m_bWireFrame ^= true;
	}

}

void Gameprocess::SetLight()
{
	Vector4 dir = Vector4(1, -3, 2, 0);
	Vector4 dif = Vector4(1, 1, 1, 0);
	Vector4 amb = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	GRAPHICENGINE::GetDirectionLightData(XMVector4Normalize(-dir), dif, amb);
}

LRESULT CALLBACK Gameprocess::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//switch (message)
	//{
	//case WM_PAINT:
	////{
	////	PAINTSTRUCT ps;
	////	HDC hdc = BeginPaint(hWnd, &ps);
	////	// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
	////	EndPaint(hWnd, &ps);
	////}
	//break;
	//case WM_MOUSEHOVER:
	//	Mouse::ProcessMessage(message, wParam, lParam);
	//	break;
	//case WM_SYSKEYUP:
	//	Keyboard::ProcessMessage(message, wParam, lParam);
	//	break;
	//case WM_SYSKEYDOWN:
	//	Keyboard::ProcessMessage(message, wParam, lParam);
	//	break;

	//case WM_DESTROY:
	//	PostQuitMessage(0);
	//	break;
	//default:
	//	return DefWindowProc(hWnd, message, wParam, lParam);
	//}

	switch (message)
	{
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	case WM_SYSKEYDOWN:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Gameprocess::Finalize()
{
	GRAPHICENGINE::Release();
	GRAPHICENGINE::ReleaseFbxLoader();
	CoUninitialize();
}

