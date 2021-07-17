#include "Material.h"

Material::Material()
	: m_TexOn(false), m_NorOn(false), m_MaskOn(false), m_PBROn(false), m_EmissiveOn(false)
{
	// 장치 정보 받아오기
	//m_pDevCopy = D3DDevice::GetInstance()->GetDevice();
	//m_pDevConCopy = D3DDevice::GetInstance()->GetDeviceContext();
}

Material::~Material()
{

}

void Material::SetTexturePath(string filename, TextureSort sort)
{
	// 외부에서 텍스쳐 경로 저장
	HRESULT hr;
	switch (sort)
	{
	case TextureSort::ALBEDO:
		m_AlbedoFilepath = filename;
		break;
	case TextureSort::NORMAL:
		m_NormalMapFilepath = filename;
		break;
	case TextureSort::ORM:
		m_ORMMapFilepath = filename;
		break;
	case TextureSort::MASKMAP:
		m_MaskMapFilepath= filename;
		break;
	case TextureSort::EMISSIVE:
		m_EmissiveMapFilepath = filename;
		break;
	}
}

void Material::SetShaderRes(string filename, TextureSort sort)
{
	HRESULT hr;
	switch (sort)
	{
	case TextureSort::ALBEDO:
		m_AlbedoFilepath = filename;
		hr = D3DDevice::GetInstance()->LoadTexture(m_AlbedoFilepath, &m_pAlbedoMap);
		if(SUCCEEDED(hr))
			m_TexOn = true;
		break;
	case TextureSort::NORMAL:
		m_NormalMapFilepath = filename;
		hr = D3DDevice::GetInstance()->LoadTexture(m_NormalMapFilepath, &m_pNormalMap);
		if (SUCCEEDED(hr))
			m_NorOn = true;
		break;
	case TextureSort::ORM:
		m_ORMMapFilepath = filename;
		hr = D3DDevice::GetInstance()->LoadTexture(m_ORMMapFilepath, &m_pORMMap);
		if (SUCCEEDED(hr))
			m_PBROn = true;
		break;
	case TextureSort::MASKMAP:
		m_MaskMapFilepath = filename;
		hr = D3DDevice::GetInstance()->LoadTexture(m_MaskMapFilepath, &m_MaskMap);
		if (SUCCEEDED(hr))
			m_MaskOn = true;
		break;
	case TextureSort::EMISSIVE:
		m_EmissiveMapFilepath = filename;
		hr = D3DDevice::GetInstance()->LoadTexture(m_EmissiveMapFilepath, &m_pEmissiveMap);
		if (SUCCEEDED(hr))
			m_EmissiveOn = true;
		break;
	}
}

string Material::GetTexturePath(TextureSort sort)
{
	string result;
	switch (sort)
	{
	case TextureSort::ALBEDO:
		result = m_AlbedoFilepath;
		break;
	case TextureSort::NORMAL:
		result = m_NormalMapFilepath;
		break;
	case TextureSort::ORM:
		result = m_ORMMapFilepath;
		break;
	case TextureSort::MASKMAP:
		result = m_MaskMapFilepath;
		break;
	case TextureSort::EMISSIVE:
		result = m_EmissiveMapFilepath;
		break;
	}

	return result;
}

void Material::SetLinkNodename(string nodename)
{
	m_LinkNodename = nodename;
}

std::string Material::GetLinkNodename()
{
	return m_LinkNodename;
}

ID3D11ShaderResourceView* Material::GetShaderRes(TextureSort sort)
{
	ID3D11ShaderResourceView* ptex = nullptr;
	switch (sort)
	{
	case Material::TextureSort::ALBEDO:
		ptex = m_pAlbedoMap;
		break;
	case Material::TextureSort::NORMAL:
		ptex = m_pNormalMap;
		break;
	case Material::TextureSort::ORM:
		ptex = m_pORMMap;
		break;
	case Material::TextureSort::MASKMAP:
		ptex = m_MaskMap;
		break;
	case Material::TextureSort::EMISSIVE:
		ptex = m_pEmissiveMap;
		break;
	}

	return ptex;
}
