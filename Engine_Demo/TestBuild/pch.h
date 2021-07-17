// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

//#ifdef UNICODE
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
//#else
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif


#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#pragma warning(disable:4996)

#pragma comment(lib, "3DGraphicsEngine")
#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "flatbuffers")
// serialize, deserialize
#include <flatbuffers/flexbuffers.h>
#undef min	//define되있지않지만 undef하지않으면 컴파일에러가 발생한다
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"

// 엔진 관련
// 여기에 미리 컴파일하려는 헤더 추가
#include "3DGraphicsEngine.h"
#include "Terrain.h"
#include "framework.h"
#include "Resource.h"
#include "Gameprocess.h"

// 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <Vfw.h>
#include "locale.h"

// Generate Code
#include "Model.h"
#include "Map.h"
#include "Player.h"
#include "Input.h"

#endif //PCH_H
