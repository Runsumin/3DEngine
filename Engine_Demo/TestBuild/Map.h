///
///
///
///
///
/// Map Generate
/// [2021/06/17/RUNSUMIN]
#pragma once
struct WorldPosData
{
	int Index;
	Vector3 Pos;
	Vector3 Rot;
	Vector3 Scl;
};

class Map
{
public:
	Map();
	~Map();
public:
	void InitializeMapData();
private:
	// 1. �� ���� �޾ƿ���
	void DeSerializeMapData();
	// 2.
	void IndexingModelList();
	// 3.
	void SetMapData();

private:
	vector<GRAPHICENGINE::IModelInterface*> m_pModelList;	// �� ����Ʈ
	vector<WorldPosData> m_WorldData;						// ��ġ ����
	vector<string>	m_IndexList;							// �ε���

	string ConvertTCharToString(const TCHAR* ptsz);
public:
	void UpdatMap(float time);
};

