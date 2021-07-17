//
//
//
// UI 클래스
// [2020/09/06 RUNSUMIN]
#pragma once
#include "DxDefine.h"
#include "Shader.h"
#include "Sprite.h"
#include "EngineInterface.h"

class UI : public GRAPHICENGINE::IUiInterface
{
public:
	UI(Vector3 Pos, Vector3 Rot, string filename);	///  InterFace 전용 생성자
	UI(Vector3 pos, Vector3 scl, ID3D11ShaderResourceView* ptex);	/// RenderTarget 전용
	~UI();
public:
	/// RenderTarget 전용
	void DrawUI_RT(ID3D11ShaderResourceView* ptex, float drawsizeX, float drawsizeY);
	void DrawUI_RT_Bloom(ID3D11ShaderResourceView* ptex, float drawsizeX, float drawsizeY);
	void DrawUI_RT_FINAL(ID3D11ShaderResourceView* pResult, ID3D11ShaderResourceView* pAddTex,float drawsizeX, float drawsizeY);
	void DrawUI_RT_Blur_ACC(ID3D11ShaderResourceView* pResult, ID3D11ShaderResourceView* pAddTex,float drawsizeX, float drawsizeY);
	void DrawUI_RT_Blur(ID3D11ShaderResourceView* pResult, float drawsizeX, float drawsizeY, float SampleCount);

private:
	bool	m_DrawScreen;
	Sprite* m_pSprite;

	virtual void UpdateUI(Vector3 pos, Vector3 vScl) override; 	// Default. Pos = (0,0,1) ; Rot = (1,1,0)
	virtual void DrawUI() override;
	virtual	DWORD GetTextureWidth() override;
	virtual	DWORD GetTextureHeight() override;
	virtual void SetUISize(float x, float y) override;
	virtual void SetOverlayColorOnUI(Vector4 color) override;
	virtual void ReleaseUI() override;
	virtual void OnUi() override;
	virtual void OffUi() override;
	
};

