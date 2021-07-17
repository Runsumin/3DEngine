#include "pch.h"
#include "Map.h"
#pragma warning(disable:4996)
Map::Map()
{

}

Map::~Map()
{

}

void Map::InitializeMapData()
{
	DeSerializeMapData();
	IndexingModelList();
	SetMapData();
}

void Map::DeSerializeMapData()
{
	string m_IndexList = "data.meta";
	string data;
	flatbuffers::LoadFile(m_IndexList.c_str(), true, &data);

	flexbuffers::Builder builder;

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();

	auto datalist = m["WorldPosData"].AsVector();

	int dataindex;
	Vector3 pos;
	Vector3 rot;
	Vector3 scl;

	// 데이터 로드...
	int cnt = datalist.size();
	int index = 0;

	for (index = 0; index < cnt; index++)
	{
		WorldPosData data;
		// index
		data.Index = datalist[index].AsInt64();
		index++;
		// pos
		data.Pos.x = datalist[index].AsTypedVector()[0].AsFloat();
		data.Pos.y = datalist[index].AsTypedVector()[1].AsFloat();
		data.Pos.z = datalist[index].AsTypedVector()[2].AsFloat();
		index++;
		// rot
		data.Rot.x = datalist[index].AsTypedVector()[0].AsFloat();
		data.Rot.y = datalist[index].AsTypedVector()[1].AsFloat();
		data.Rot.z = datalist[index].AsTypedVector()[2].AsFloat();
		index++;
		//scl
		data.Scl.x = datalist[index].AsTypedVector()[0].AsFloat();
		data.Scl.y = datalist[index].AsTypedVector()[1].AsFloat();
		data.Scl.z = datalist[index].AsTypedVector()[2].AsFloat();

		m_WorldData.push_back(data);
	}

}

void Map::IndexingModelList()
{
	cout << "Model Load.." << endl;

	FILE* fp = _tfopen(L"data.ini", L"rt");
	TCHAR line[256] = L"";
	TCHAR token[256] = L"";
	setlocale(LC_ALL, "");

	while (1)
	{
		if (feof(fp)) break;

		_fgetts(line, 256, fp);	//라인의 '공백'때문에 fscanf 는 쓸수 없다.. 
		_stscanf(line, L"%s", token);

		if (_tcsnicmp(token, L"//", 2) == 0) continue;

		m_IndexList.push_back(ConvertTCharToString(token));
	}

	for (int i = 0; i < m_IndexList.size(); i++)
	{
		GRAPHICENGINE::CreateModelObject(m_IndexList[i]);
		cout << m_IndexList[i] << " Load ComPlete" << endl;
	}

	cout << "Texture Data Load.." << endl;
	GRAPHICENGINE::CreateModelMaterial_Texture();
}

void Map::SetMapData()
{
	cout << "Map Data Load.." << endl;
	int cnt = m_WorldData.size();
	int resize = cnt / 5;
	for (int i = 0; i < resize; i++)
	{
		// 모델 이름 by index
		int index = m_WorldData[i].Index;
		string filename = m_IndexList[index];

		GRAPHICENGINE::IModelInterface* pModel;

		GRAPHICENGINE::SetModelObject(&pModel, filename);

		Matrix trans, scl, Rot;

		trans = Matrix::CreateTranslation(m_WorldData[i].Pos);
		scl = Matrix::CreateScale(m_WorldData[i].Scl.x, m_WorldData[i].Scl.y, m_WorldData[i].Scl.z);
		Rot = XMMatrixRotationRollPitchYawFromVector(m_WorldData[i].Rot);

		pModel->InitFbxModel();
		pModel->SetUserMatrix(scl * Rot * trans);
		m_pModelList.push_back(pModel);
	}
}

std::string Map::ConvertTCharToString(const TCHAR* ptsz)
{
	int len = wcslen((wchar_t*)ptsz);
	char* psz = new char[2 * len + 1];
	wcstombs(psz, (wchar_t*)ptsz, 2 * len + 1);
	std::string s = psz;
	delete[] psz;
	return s;
}

void Map::UpdatMap(float time)
{
	int cnt = m_pModelList.size();

	for (int i = 0; i < cnt; i++)
	{
		m_pModelList[i]->FbxModelUpdate(time);
	}
}

