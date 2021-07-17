//
//
//
// 인터페이스 - 오픈될 정보들
// 추후에 다른 API 와의 확정성을 고려하여 제작
// [2020/12/07 RUNSUMIN]
#pragma once
#include <windows.h>	// 윈도우 핸들..
#include <SimpleMath.h>	// 수학 헤더

// 렌더 모드
enum class RenderState
{
	e_CullBack, //Default
	e_CullFront,
	e_WireFrame,
	e_Solid
};

namespace GRAPHICENGINE
{
	// 싱글턴 변경은 추후에 고려
	class IDevInterface abstract
	{
	public:
		/// Device
		// 엔진 셋팅
		virtual bool EngineSetUp(HWND hwnd)	abstract;	
		// 그리기 시작
		virtual void BeginRend(Vector4 backbuffercolor)	abstract;
		// 그리기 종료
		virtual void EndRend() abstract;
		// 엔진 해제
		virtual void Release() abstract;
		// 엔진 타임 받아오기
		virtual float GetEngineTime() abstract;
		// 렌더 모드 변경
		virtual void ChangeRenderState(RenderState renderstate) abstract;
		// 창 크기 변경

		// 창 모드 변경

		/// Camera
	};

	class ICameraInterface abstract
	{

	};

}

