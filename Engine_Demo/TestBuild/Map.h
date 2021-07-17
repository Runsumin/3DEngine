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
	// 1. ¸Ê Á¤º¸ ¹Þ¾Æ¿À±â
	void DeSerializeMapData();
	// 2.
	void IndexingModelList();
	// 3.
	void SetMapData();

private:
	vector<GRAPHICENGINE::IModelInterface*> m_pModelList;	// ¸ðµ¨ ¸®½ºÆ®
	vector<WorldPosData> m_WorldData;						// À§Ä¡ Á¤º¸
	vector<string>	m_IndexList;							// ÀÎµ¦½Ì

	string ConvertTCharToString(const TCHAR* ptsz);
public:
	void UpdatMap(float time);
};

