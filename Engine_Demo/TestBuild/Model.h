#pragma once
#include "Input.h"

class Model_default
{
public:
	Model_default();
	~Model_default();
public:
	GRAPHICENGINE::IModelInterface* m_pModel;
	GRAPHICENGINE::IMaterialInterface* m_pBlack;
	GRAPHICENGINE::IMaterialInterface* m_pGreen;

	// 재질 정보 생성
	vector<GRAPHICENGINE::IMaterialInterface*> m_pBlackList;
	vector<GRAPHICENGINE::IMaterialInterface*> m_pDevilMaskMapList;
	vector<GRAPHICENGINE::IMaterialInterface*> m_pDevilORMList;
	vector<GRAPHICENGINE::IMaterialInterface*> m_pHouseMat;

private:
protected:
public:
	void InitModel(string filename, Matrix Mat);
	void InitMaterial(int type);
	void Update(float enignetime);
	void ChangeMaterial();
	Matrix GetMarbyname(string name);
	void SetMatrix(Matrix mat);

	float m_Angle;

	bool m_b;
};

