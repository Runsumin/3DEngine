//
//
//
// DX 초기화 위한 프레임 워크 생성
/// [2020/07/20 RUNSUMIN]

#include "DxDefine.h"
#include "3DGraphicsEngine.h"
#include "GameProcess.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    GameProcess* pGameprocess = new GameProcess();
    pGameprocess->Initialize(hInstance);
    pGameprocess->GameLoop();
    pGameprocess->Finalize();

    return 0;
}

