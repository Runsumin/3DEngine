#include "UI.h"

UI::UI(Vector3 Pos, Vector3 Rot, string filename)
{
	this->m_pSprite = new Sprite(Pos, Rot, filename);
	this->m_pSprite->InitSprite();
	//this->m_pSprite->InitShader();
	this->m_DrawScreen = true;
}

UI::UI(Vector3 pos, Vector3 scl, ID3D11ShaderResourceView* ptex)
{
	this->m_pSprite = new Sprite(pos, scl, ptex);
	this->m_pSprite->InitSprite();
	//this->m_pSprite->InitShader();
	this->m_DrawScreen = true;
}

UI::~UI()
{
	ReleaseUI();
	if (m_pSprite)
	{
		delete m_pSprite;
		m_pSprite = nullptr;
	}
}

void UI::UpdateUI(Vector3 pos, Vector3 vScl)
{
	m_pSprite->UpdateSprite(pos, vScl);
}

void UI::DrawUI()
{
	if (m_DrawScreen == true)
	{
		m_pSprite->DrawSprite();
	}
}

DWORD UI::GetTextureWidth()
{
	return m_pSprite->m_Width;
}

DWORD UI::GetTextureHeight()
{
	return m_pSprite->m_Height;
}

void UI::SetUISize(float x, float y)
{
	m_pSprite->SetChangeSize(x, y);
}

void UI::SetOverlayColorOnUI(Vector4 color)
{
	m_pSprite->m_OverRayCol = color;
}

void UI::ReleaseUI()
{
	m_pSprite->ReleaseSprite();
}

void UI::OnUi()
{
	m_DrawScreen = true;
}

void UI::OffUi()
{
	m_DrawScreen = false;
}

void UI::DrawUI_RT(ID3D11ShaderResourceView* ptex, float drawsizeX, float drawsizeY)
{
	m_pSprite->DrawSprite_Rt(ptex, drawsizeX, drawsizeY);
}

void UI::DrawUI_RT_Bloom(ID3D11ShaderResourceView* ptex, float drawsizeX, float drawsizeY)
{
	m_pSprite->DrawSprite_Bloom(ptex, drawsizeX, drawsizeY);
}

void UI::DrawUI_RT_FINAL(ID3D11ShaderResourceView* pResult, ID3D11ShaderResourceView* pAddTex, float drawsizeX, float drawsizeY)
{
	m_pSprite->DrawSprite_FINAL(pResult, pAddTex, drawsizeX, drawsizeY);
}

void UI::DrawUI_RT_Blur_ACC(ID3D11ShaderResourceView* pResult, ID3D11ShaderResourceView* pAddTex, float drawsizeX, float drawsizeY)
{
	m_pSprite->DrawSprite_Blur_ACC(pResult, pAddTex, drawsizeX, drawsizeY);
}

void UI::DrawUI_RT_Blur(ID3D11ShaderResourceView* pResult, float drawsizeX, float drawsizeY, float SampleCount)
{
	m_pSprite->DrawSprite_Blur(pResult, drawsizeX, drawsizeY, SampleCount);
}
