#include "Fbx_MeshData.h"

Fbx_MeshData::Fbx_MeshData()
	:m_AutoTextureSet(false), m_BiggstX(0.0f), m_BiggstY(0.0f), m_BiggstZ(0.0f),
	m_leastX(0.0f), m_leastY(0.0f), m_leastZ(0.0f)
{

}

Fbx_MeshData::~Fbx_MeshData()
{
	ReleaseModelData();
}

void Fbx_MeshData::SetMaterialDataAuto()
{
	// 머테리얼 미리 생성...<설정한 텍스쳐 개수에 맞게>
	// diffuse와 Normal은 기본적으로 설정해준다.
	// 추후 ORM 추가 예정.
	int texcount = m_Texfilename.size();
	int norcount = m_Norfilename.size();
	int ormcount = m_ORMfilename.size();

	/// 자동 등록 버전...  - 텍스쳐와 노말 둘 다 존재 - OK
	// Material 중복 생성 방지...
	if (texcount - norcount == 0)
	{
		m_pMaterialList.resize(texcount);
		for (int i = 0; i < texcount; i++)
		{
			Material* tmp = new Material();
			tmp->SetShaderRes(m_TexPath + m_Texfilename[i], GRAPHICENGINE::IMaterialInterface::TextureSort::ALBEDO);
			tmp->SetShaderRes(m_TexPath + m_Norfilename[i], GRAPHICENGINE::IMaterialInterface::TextureSort::NORMAL);
			tmp->SetShaderRes(m_TexPath + m_ORMfilename[i], GRAPHICENGINE::IMaterialInterface::TextureSort::ORM);
			tmp->SetLinkNodename(m_LinkNodeNameList[i]);
			m_pMaterialList[i] = tmp;
		}
		m_AutoTextureSet = true;
	}
	/// 텍스쳐만 존재 - X
	else if (texcount - norcount != 0)
	{
		m_pMaterialList.resize(texcount);
		for (int i = 0; i < texcount; i++)
		{
			Material* tmp = new Material();
			tmp->SetShaderRes(m_TexPath + m_Texfilename[i], GRAPHICENGINE::IMaterialInterface::TextureSort::ALBEDO);
			tmp->SetLinkNodename(m_LinkNodeNameList[i]);
			m_pMaterialList[i] = tmp;
		}
		/// 임시 - 맵리소스는 일단 텍스쳐만 들어가 있다.
		switch (m_MeshFilterSort)
		{
		case MeshFilter::eMeshfiltersort::SkinnedMesh:
			m_AutoTextureSet = false;
			break;
		case MeshFilter::eMeshfiltersort::StaticMesh:
			m_AutoTextureSet = true;
			break;
		}
	}
	else
	{
		m_pMaterialList.resize(texcount);
		m_AutoTextureSet = false;
	}
}

void Fbx_MeshData::ReleaseModelData()
{
	deletevector(m_pFbxDataList);
	m_pFbxDataList.clear();
	deletevector(m_pAllBoneList);
	m_pAllBoneList.clear();
	m_pBoneList_Hierarchy.clear();
	deletevector(m_pAniDataList);
	m_pAniDataList.clear();
}