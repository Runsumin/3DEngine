#include "GameProcess.h"

GameProcess::GameProcess()
	:m_EngineTime(0.f)
{

}

GameProcess::~GameProcess()
{

}

HRESULT GameProcess::Initialize(HINSTANCE hInstance)
{
	/// Win32 관련
	// 윈도 클래스
	wchar_t szAppName[] = L"RUNSUMIN 3D Graphics Engine";
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = GameProcess::WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	// 윈도 클래스 등록
	RegisterClass(&wndclass);

	// 윈도 생성
	m_hWnd = CreateWindow(
		szAppName,
		szAppName,
		WS_OVERLAPPEDWINDOW,
		0, 0, m_ScreenWidth, m_ScreenHeight,
		NULL, NULL, hInstance, NULL);

	if (!m_hWnd) return FALSE;

	// 컴객체 초기화
	if (FAILED(CoInitialize(NULL)))
	{
		return S_FALSE;
	}


	/// 테스트
	/// 카메라
	Camera::GetInstance()->InitializeCamera();

	/// 디바이스
	D3DDevice::GetInstance()->DXSetup(m_hWnd);

	m_BackBufferColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1);
	//m_BackBufferColor = XMFLOAT4(1, 0, 1, 1);

	/// 텍스트 
	DrawText_Debug::GetInstance()->CreateFont(D3DDevice::GetInstance()->GetDevice());

	m_FontColor = XMFLOAT4(255, 255, 0, 1);

	/// 모델
	//TCHAR filename[256] = L"../Data/Skinning/Arm/Tube_Skin_short.ase";
	TCHAR filename[256] = L"../Data/Skinning/Bear/Bear.ase";
	//TCHAR filename[256] = L"../Data/SmoothingGroob_Test.ase";
	//TCHAR filename[256] = L"../Data/rabbit_girl/RGirl_Run.ase";
	//TCHAR filename[256] = L"../Data/rabbit_girl/RGirl_Attack_w.ase";
	//TCHAR filename[256] = L"../Data/SuminSample/Run_Ex_Sample.ase";
	//TCHAR filename2[256] = L"../Data/Skinning/Bear/Bear.ase";

	m_pObject = new ModelObject();

	m_pObject->initModel();

	m_pObject->initShader();

	m_pObject->LoadModelFromFile(filename, D3DDevice::GetInstance()->GetDevice(), D3DDevice::GetInstance()->GetDeviceContext());

	/// 그리드
	m_pGrid = new Grid();

	m_pGrid->InitGrid();

	/// 좌표축
	m_pAxis = new Axis();

	m_pAxis->InitAxis();

	// 생성된 윈도를 화면에 표시
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	HRESULT hr = S_OK;
	return hr;
}

void GameProcess::GameLoop()
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
			UpdateAll();
			RenderAll();

		}
	}
}

void GameProcess::UpdateAll()
{
	m_EngineTime = D3DDevice::GetInstance()->GetEngineTime();

	Camera::GetInstance()->UpdateCamera(m_EngineTime);

	m_pGrid->Updata();
	m_pAxis->Update();

	/// 렌더 모드 설정.... 
	D3DDevice::GetInstance()->GetDeviceContext()->RSSetState(D3DDevice::GetInstance()->m_RState[RS_CULLBACK]);

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
	 	 	D3DDevice::GetInstance()->GetDeviceContext()->RSSetState(D3DDevice::GetInstance()->m_RState[RS_WIREFRM]);
	}

	D3DDevice::GetInstance()->GetDeviceContext()->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON], 0);
	D3DDevice::GetInstance()->GetDeviceContext()->PSSetSamplers(0, 1, &D3DDevice::GetInstance()->m_pSampler[SS_DEFAULT]);

	/// 모델 업데이트 
	m_pObject->ModelUpdate(m_EngineTime);

	//프로그램 종료 처리.
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) PostQuitMessage(WM_QUIT);
}

void GameProcess::RenderAll()
{
	D3DDevice::GetInstance()->ClearBackBufer(D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0,m_BackBufferColor);

	m_pAxis->Render();
	m_pGrid->Render();
	m_pObject->ModelDraw(m_EngineTime);
	DrawText_Debug::GetInstance()->DrawTextEx(0, 0, m_FontColor, L"2020 RUNSUMIN 3D ENGINE");
	Camera::GetInstance()->CameraInfo(m_ScreenWidth, m_ScreenHeight);

	D3DDevice::GetInstance()->Flip();
}

void GameProcess::Finalize()
{
	D3DDevice::GetInstance()->Release();
	CoUninitialize();
}

LRESULT CALLBACK GameProcess::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
