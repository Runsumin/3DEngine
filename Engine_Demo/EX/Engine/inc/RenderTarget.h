///
/// ·»´õ Å¸°Ù »ý¼º Å¬·¡½º
/// [2020/11/09 RUNSUMIN]
///
/// ·»´õÅ¸°Ù Å¬·¡½º ¼öÁ¤
/// [2021/04/13 RUNSUMIN]
#pragma once

#include "DxDefine.h"
#include "D3DDevice.h"

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	ID3D11Device* m_pDevCopy;					// µð¹ÙÀÌ½º Ä«ÇÇ
	ID3D11DeviceContext* m_pDevConCopy;			// µð¹ÙÀÌ½º ÄÁÅØ½ºÆ® Ä«ÇÇ

	ID3D11Texture2D* m_pRtTex;					// ·»´õÅ¸°Ù¿¡ ±×·ÁÁú µ¥ÀÌÅÍ
	ID3D11RenderTargetView* m_pRtView;			// ·»´õÅ¸°Ù ºä
	ID3D11ShaderResourceView* m_pRtShaderTex;	// ¼ÎÀÌ´õ ¸®¼Ò½º

	/////
	ID3D11Texture2D* m_pRtTexBuff;				// ±íÀÌ/½ºÅÙ½Ç ¹öÆÛ(ÅØ½ºÃÄ)
	ID3D11DepthStencilView* m_pRtViewScene;		// ±íÀÌ/½ºÅÙ½Ç ºä

public:
	void CreateRenderTarget(SIZE size, DXGI_FORMAT fmt , DXGI_FORMAT dsfmt);
	void UpdataRt(Vector4 col);
	void RestoreRt();

private:
	// ·»´õÅ¸°Ù¿ë ºó ÅØ½ºÃÄ.
	HRESULT CreateRtTexture(UINT width, UINT height, DXGI_FORMAT format);
	// ·»´õÅ¸°Ù¿ë ¸®¼Ò½ººä.
	HRESULT CreateRtView(DXGI_FORMAT format);
	// ·»´õÅ¸°Ù¿ë ¼ÎÀÌ´õ ¸®¼Ò½º ºä.
	HRESULT CreateRtShaderView(DXGI_FORMAT format);
	// ·»´õÅ¸°Ù¿ë ±íÀÌ/½ºÅÙ½Ç ºä.
	HRESULT CreateRtDepthStencil(UINT width, UINT height, DXGI_FORMAT format);

	void ReleaseRt();

protected:
};

