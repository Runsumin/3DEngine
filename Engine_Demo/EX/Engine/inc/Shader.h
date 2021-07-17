//
//
//
// 
// [2020/07/26 RUNSUMIN]
#pragma once

#include "DxDefine.h"
#include "D3DDevice.h"

class Shader
{
public:
	Shader();
	~Shader();

public:
	// 멤버변수들
	ID3D11VertexShader* m_pVS;			// 정점 셰이더 인터페이스
	ID3D11PixelShader* m_pPS;			// 픽셀 쎄이더 인터페이스
	ID3DBlob* m_pVSCode;				// 컴파일 코드 개체
	ID3D11InputLayout* m_pVBLayout;		// 정점 입력 레이아웃

public:
	// 최종 셰이더 로드 함수
	HRESULT ShaderLoad(const TCHAR* fxname, ID3D11Device* pDevice);

	// 정점 입력구조 생성
	HRESULT CreateVBLayOut(D3D11_INPUT_ELEMENT_DESC layout[], UINT layoutsize, ID3D11Device* pDevice);

	// 동적 상수버퍼 생성
	HRESULT CreateDynamicConstantBuffer(UINT size, LPVOID pdata, ID3D11Buffer** ppConstantbuff,
		ID3D11Device* pDevice);

	// 동적 상수버퍼 갱신
	HRESULT UpdateDynamicConstantBuffer(ID3D11DeviceContext* pDXDC, ID3D11Resource* pBuff, LPVOID pData,
		UINT size);

public:
	void ShaderRelease();


};

