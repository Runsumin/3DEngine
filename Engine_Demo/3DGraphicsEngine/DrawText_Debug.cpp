//
//
//
// 디버깅용 화면 문자 출력 클래스
// [2020/07/23 RUNSUMIN]

#include "DrawText_Debug.h"
#include "D3DDevice.h"

DrawText_Debug* DrawText_Debug::m_Instance = nullptr;

DrawText_Debug* DrawText_Debug::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new DrawText_Debug();
	}
	return m_Instance;
}


DrawText_Debug::DrawText_Debug()
{

}

DrawText_Debug::~DrawText_Debug()
{
	if (m_pFont)
	{
		delete m_pFont;
		m_pFont = nullptr;
	}

	for(auto font : m_pFontList)
	{
		delete font;
		font = nullptr;
	}
}

void DrawText_Debug::Finalize()
{
	if (m_Instance)
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
}

void DrawText_Debug::CreateFont()
{
	ID3D11DeviceContext* _pDXDC = nullptr;

	D3DDevice::GetInstance()->m_Device->GetImmediateContext(&_pDXDC);
	m_pFontBatch = new SpriteBatch(_pDXDC);

	// 폰트 유동적으로 생성 가능하게....
	//DirectX Toolkit : Sprite Font 객체 생성.
	const TCHAR* _filename = L"../EX/Font/RoundedTypeface.sfont";
	///const TCHAR* filename = L"../EX/Font/Nickname.spritefont";
	try 
	{
		m_pFont = new SpriteFont(D3DDevice::GetInstance()->m_Device, _filename);
		m_pFont->SetLineSpacing(14.0f);
		m_pFont->SetDefaultCharacter('_');
	}
	catch (std::exception& e)
	{
		TCHAR _msg[1024] = L"";
		::mbstowcs(_msg, e.what(), strlen(e.what()));
	}

	//사용후, 장치목록 해제.
	SAFE_RELEASE(_pDXDC);
}

int DrawText_Debug::CreateFont(const TCHAR* filename, float LineSpacing)
{
	ID3D11DeviceContext* _pDXDC = nullptr;

	D3DDevice::GetInstance()->m_Device->GetImmediateContext(&_pDXDC);
	m_pFontBatch = new SpriteBatch(_pDXDC);

	// 폰트 유동적으로 생성 가능하게....
	//DirectX Toolkit : Sprite Font 객체 생성.
	try 
	{
		SpriteFont* _font = new SpriteFont(D3DDevice::GetInstance()->m_Device, filename);
		_font->SetLineSpacing(LineSpacing);
		_font->SetDefaultCharacter('_');
		m_pFontList.push_back(_font);
	}
	catch (std::exception& e)
	{
		TCHAR _msg[1024] = L"";
		::mbstowcs(_msg, e.what(), strlen(e.what()));
	}

	//사용후, 장치목록 해제.
	SAFE_RELEASE(_pDXDC);

	return (m_pFontList.size() - 1);
}

void DrawText_Debug::DrawTextEx(int x, int y, XMFLOAT4 col, const TCHAR* msg, ...)
{
	const DWORD _size = 2048;

	TCHAR _buff[_size] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(_buff, msg, vl);
	va_end(vl);

	m_pFontBatch->Begin();
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_OFF], 0);
	XMFLOAT2 _pos = XMFLOAT2((float)x, (float)y);
	m_pFont->DrawString(m_pFontBatch, _buff, XMLoadFloat2(&_pos), XMLoadFloat4(&col), 0, XMVECTOR());					//원본 크기.
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON], 0);
	m_pFontBatch->End();
}

void DrawText_Debug::DrawTextEx(int x, int y, Vector4 col, float fontSize, const TCHAR* msg, va_list valist)
{
	const DWORD _size = 2048;

	TCHAR _buff[_size] = L"";

	_vstprintf(_buff, msg, valist);

	m_pFontBatch->Begin();
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_OFF], 0);
	XMFLOAT2 _pos = XMFLOAT2((float)x, (float)y);
	m_pFont->DrawString(m_pFontBatch, _buff, XMLoadFloat2(&_pos), XMLoadFloat4(&col), 0, XMVECTOR(), fontSize * 0.01f);                    //원본 크기.
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON], 0);
	m_pFontBatch->End();
}

void DrawText_Debug::DrawTextEx(int fontIndex, int x, int y, Vector4 col, float fontSize, const TCHAR* msg, va_list valist)
{
	if (fontIndex >= m_pFontList.size())
		return;

	const DWORD _size = 2048;

	TCHAR _buff[_size] = L"";

	_vstprintf(_buff, msg, valist);

	m_pFontBatch->Begin();
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_OFF], 0);
	XMFLOAT2 _pos = XMFLOAT2((float)x, (float)y);
	m_pFontList[fontIndex]->DrawString(m_pFontBatch, _buff, XMLoadFloat2(&_pos), XMLoadFloat4(&col), 0, XMVECTOR(), fontSize * 0.01f);                    //원본 크기.
	D3DDevice::GetInstance()->GetDeviceContext()->OMSetDepthStencilState(D3DDevice::GetInstance()->m_DSState[DS_DEPTH_ON], 0);
	m_pFontBatch->End();
}

Vector2 DrawText_Debug::GetSpriteFontSize(const TCHAR* msg)
{
	return m_pFont->MeasureString(msg) * 0.1f;
}

Vector2 DrawText_Debug::GetSpriteFontSize(int fontIndex, const TCHAR* msg)
{
	if (fontIndex < m_pFontList.size())
	{
		return m_pFontList[fontIndex]->MeasureString(msg) * 0.1f;
	}
	return Vector2::Zero;
}

DirectX::SimpleMath::Vector2 DrawText_Debug::GetSpriteFontSize(const TCHAR* msg, float fontSize)
{
	return m_pFont->MeasureString(msg) * fontSize * 0.01f;
}

DirectX::SimpleMath::Vector2 DrawText_Debug::GetSpriteFontSize(int fontIndex, const TCHAR* msg, float fontSize)
{
	if (fontIndex < m_pFontList.size())
	{
		return m_pFontList[fontIndex]->MeasureString(msg) * fontSize * 0.01f;
	}
	return Vector2::Zero;
}
