//
//
//
// 디버깅용 화면 문자 출력 클래스
// [2020/07/23 RUNSUMIN]
#pragma once

#include "DxDefine.h"

class DrawText_Debug
{
private:
	static DrawText_Debug* m_Instance;
public:
	static DrawText_Debug* GetInstance();

private:
	SpriteBatch* m_pFontBatch;
	SpriteFont* m_pFont;
	std::vector<SpriteFont*> m_pFontList;

private:
	DrawText_Debug();
	~DrawText_Debug();

public:
	void Finalize();
	void CreateFont();
	int CreateFont(const TCHAR* filename, float LineSpacing);
	void DrawTextEx(int x, int y, XMFLOAT4 col, const TCHAR* msg, ...);
	//void DrawTextEx(int x, int y, Vector4 col, const TCHAR* msg, va_list valist);
	void DrawTextEx(int x, int y, Vector4 col, float fontSize, const TCHAR* msg, va_list valist);
	void DrawTextEx(int fontIndex, int x, int y, Vector4 col, float fontSize, const TCHAR* msg, va_list valist);

	Vector2 GetSpriteFontSize(const TCHAR* msg);
	Vector2 GetSpriteFontSize(const TCHAR* msg, float fontSize);
	Vector2 GetSpriteFontSize(int fontIndex, const TCHAR* msg);
	Vector2 GetSpriteFontSize(int fontIndex, const TCHAR* msg, float fontSize);
};

