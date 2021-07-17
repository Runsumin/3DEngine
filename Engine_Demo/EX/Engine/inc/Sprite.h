//
//
// 
// 화면에 뿌려줄 텍스쳐 한장의 클래스
// 회전은 추후 추가 예정... 고려사항이 많다.
// [2021/02/06 RUNSUMIN]
#pragma once
#include "DxDefine.h"
#include "D3DDevice.h"
#include "Shader.h"

struct _VTX
{
	Vector3 pos;
	Vector2 tex;
};

struct OVERLAYCOLOR
{
	Vector4 color;
};
class Sprite
{
public:
	Sprite(Vector3 pos);
	/// interface
	Sprite(Vector3 pos, Vector3 scl, string TexFileName);	
	/// RenderTarget
	Sprite(Vector3 pos, Vector3 scl, ID3D11ShaderResourceView* ptex);
	~Sprite();

private:

	string m_Filename;
	Shader* m_pShader;							// 셰이더 정보
	CBDEFAULT   m_CbDef;						// 기본 운용 행렬 정보
	OVERLAYCOLOR  m_CbMtl;						// 재질 정보

	ID3D11Buffer* m_pCbDefBuf;					// 기본 운용정보 버퍼
	ID3D11Buffer* m_pCbMtlBuf;					// 재질 정보 버퍼

	ID3D11Device* m_pDevCopy;					// 디바이스 카피
	ID3D11DeviceContext* m_pDevConCopy;			// 디바이스 컨텍스트 카피

	ID3D11ShaderResourceView* m_pTex;			// 텍스쳐 정보
	D3D11_TEXTURE2D_DESC	m_TexDesc;		// 데스크 

	// 화면에 충돌 해야 할 경우는 다음에 생각하자(마우스 X)
	ID3D11Buffer* m_pVB;

	/// 뎁스 픽셀 셰이더
	ID3D11PixelShader* m_pPsDepth;
	ID3D11PixelShader* m_pPsBloom;
	ID3D11PixelShader* m_pPsFinal;
	ID3D11PixelShader* m_pPsSampling;
	ID3D11PixelShader* m_pPsBlur;

public:
	Matrix m_View;
	Matrix m_Porj;

	Vector3 m_Pos;
	Vector3 m_Scl;
	Vector3 m_Rot;

	float	m_Width;		// 가로
	float	m_Height;		// 세로
	Vector4 m_OverRayCol;	// 스프라이트 위에 엎을 색깔.

	float m_ChangeSizeX;
	float m_ChangeSizeY;

public:
	int InitSprite();
	int InitShader();
	void UpdateSprite(Vector3 pos, Vector3 scl);
	int DrawSprite();
	int ReleaseSprite();
	void SetChangeSize(float x, float y);

	/// RenderTarget 전용
	void DrawSprite_Rt(ID3D11ShaderResourceView* ptex, float drawsizeX, float drawsizeY);
	void DrawSprite_Bloom(ID3D11ShaderResourceView* ptex, float drawsizeX, float drawsizeY);
	void DrawSprite_FINAL(ID3D11ShaderResourceView* pResult, ID3D11ShaderResourceView* pAddTex, float drawsizeX, float drawsizeY);
	void DrawSprite_Blur_ACC(ID3D11ShaderResourceView* pResult, ID3D11ShaderResourceView* pAddTex, float drawsizeX, float drawsizeY);
	void DrawSprite_Blur(ID3D11ShaderResourceView* pResult, float drawsizeX, float drawsizeY, float SampleCount);

};

