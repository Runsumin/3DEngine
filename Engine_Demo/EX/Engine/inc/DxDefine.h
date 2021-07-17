//
//
//
// 
// [2020/07/23 RUNSUMIN]
#pragma once
#pragma warning(disable:4996)

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.

#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <vector>
#include <fbxsdk.h>
// include
#include <D3D11.h>
#include <d3dcompiler.h>				//	shader 컴파일러 헤더
//#include "directxmath.h"
#include <stdio.h>
#include <Tchar.h>
#include <windows.h>
#include <SimpleMath.h>
#include <DirectXTex.h>	
#include <DDSTextureLoader.h> 
#include <WICTextureLoader.h>


using namespace DirectX::SimpleMath;
using namespace std;

// linking
#pragma comment(lib,"D3D11")
#pragma comment(lib,"dxgi")
#pragma comment(lib,"d3dcompiler")
#pragma comment(lib, "flatbuffers")

#ifdef _DEBUG 
#ifdef _WIN64
#pragma comment(lib, "DirectXTex64d")	//디버그 버전.(64bit)
#elif _WIN32
#pragma comment(lib, "DirectXTexd")		//디버그 버전.
#endif
#else
#ifdef _WIN64 
#pragma comment(lib, "DirectXTex64")	//릴리즈 버전 (64bit)
#elif _WIN32 
#pragma comment(lib, "DirectXTex")		//릴리즈 버전.
#endif
#endif




//////////////////////////////////////////////////////////////////////////



//DrawText
#include <DirectXTK.h>
using namespace DirectX;

// define
#include "Define.h"

// warning
//#include "AlignedAllocationPolicy.h"


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(pBuff) if((pBuff)){ (pBuff)->Release(); (pBuff) = NULL; }
#define SAFE_DELETE(pBuff)	if((pBuff)){ delete (pBuff); (pBuff) = NULL; }
#define SAFE_DELARRY(pBuff) if((pBuff)){ delete [] (pBuff); (pBuff) = NULL; }
#endif

#define OK_SIGN	0x00000000
#define FAIL_SIGN	-1

typedef XMMATRIX		MATRIXA;
typedef XMFLOAT4X4		MATRIX;
typedef XMVECTOR		VECTOR;
typedef XMFLOAT4		VECTOR4;
typedef XMFLOAT3		VECTOR3;
typedef XMFLOAT2		VECTOR2;

//색상 타입: 2가지.
//typedef XMCOLOR		COLOR;	
typedef XMFLOAT4		COLOR;

// 깊이, 스텐실 테스트 상태

enum eDepthStencilSet
{
	DS_DEPTH_ON,		
	DS_DEPTH_OFF,		
	DS_DEPTH_WRITE_OFF,	

	//스텐실 버퍼 연산용.
	DS_DEPTH_ON_STENCIL_ON,
	DS_DEPTH_ON_STENCIL_EQUAL_KEEP,
	DS_DEPTH_ON_STENCIL_NOTEQUAL_KEEP,

	DS_DEPTH_WRITE_OFF_STENCIL_ON,
	DS_DEPTH_OFF_STENCIL_EQUAL_INCR,


	//열거 상수 최대값.
	DS_MAX_,

	//상수 재정의. MAX 값 뒷쪽에 정의해야 함. 주의.
	DS_DEPTH_ON_STENCIL_OFF = DS_DEPTH_ON,			//깊이버퍼 ON, 스텐실버퍼 OFF
	DS_DEPTH_ONLY = DS_DEPTH_ON_STENCIL_OFF,		//깊이버퍼 ON, 스텐실버퍼 OFF
};

enum eObjectRasterizerState 
{
	RS_SOLID,				//삼각형 채우기 : Fill Mode - Soild.
	RS_WIREFRM,				//삼각형 채우기 : Fill Mode - Wireframe.
	RS_CULLBACK,			//뒷면 컬링 (ON) : Back-Face Culling - "CCW" 
	RS_CULLFRONT,			//정면 컬링 (ON) : Front-Face Culling - "CW" 	
	RS_WIRECULLBACK,		//와이어 프레임 + 뒷면 컬링 (ON) 
	RS_WIRECULLFRONT,		//와이어 프레임 + 앞면 컬링 (ON) 	

	//열거 상수 최대값.
	RS_MAX_,

	//상수 재정의. MAX 값 뒷쪽에 정의해야 함. 주의.
	RS_CULL_CCW = RS_CULLBACK,
	RS_CULL_CW = RS_CULLFRONT,
	RS_WIRE_CULL_CCW = RS_WIRECULLBACK,
	RS_WIRE_CULL_CW = RS_WIRECULLFRONT,

};

enum eObjectBlendingState
{
	BS_DEFAULT,				//기본혼합 객체.
	BS_ALPHA_BLEND,			//알파 블랜딩. (Src.a + (1-Src.a))
	BS_COLOR_BLEND,			//색상 블랜딩. ( Src.Color + (1-Src.Color))
	BS_COLOR_BLEND_ONE,		//1:1 블랜딩. ( Src.Color + Dest.Color) 

	BS_MAX_,

	BS_AB_OFF = BS_DEFAULT,
	BS_AB_ON = BS_ALPHA_BLEND,
	//BS_AB_ALPHA = BS_ALPHA_BLEND,
	//BS_AB_COLOR = BS_COLOR_BLEND,
};

enum eTextureSamplerState
{
	SS_CLAMP,			//텍스처 어드레스 모드 : 클램핑 (DX 기본값) 
	SS_WRAP,			//텍스처 어드레스 모드 : 렙핑 
	SS_BOARDER,
	SS_SHADOW,
	//SS_MIRROR,
	//SS_MIRRROONCE,

	SS_MAX,
	SS_DEFAULT = SS_WRAP,	//기본 셈플러

};
