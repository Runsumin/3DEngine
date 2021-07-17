//
//
//
// 디버깅용 그리드 
// [2020/08/06 RUNSUMIN]
#pragma once
#include "DxDefine.h"
#include "Shader.h"
#include "Camera.h"
#include "EngineInterface.h"
class Grid : public GRAPHICENGINE::IGridInterface
{
public:
	struct VTX_GIRD
	{
		float x,y,z;
		Vector4 color;
	};

	/// 상수버퍼 (16바이트 정렬)
	_declspec(align(16)) struct CBGRID
	{
		XMMATRIX mWorld;
		XMMATRIX mView;
		XMMATRIX mProj;
		XMMATRIX mWVP;
	};

public:
	Grid();
	~Grid();

	virtual void InitializeGrid(int numx, int numy, float size) override;
	virtual void UpdateGrid() override;
	virtual void DrawGrid()	override;
	virtual void ReleaseGrid() override;
	virtual void ChangeGridSize(int numx, int numy, float size) override;


private:
	ID3D11Device* m_pDeviceCopy;
	ID3D11DeviceContext* m_pDeviceContextCopy;

	Shader* m_pShader;

	CBGRID m_cbGrid;

	vector<VTX_GIRD> m_Grid;
	vector<int> m_index;

	ID3D11Buffer* m_pGridVertexbuf;
	ID3D11Buffer* m_pVb;
	ID3D11Buffer* m_pIb;
	Vector3 m_Movegrid;
	int m_SizeX;
	int m_SizeY;
	int m_Length;

	/////

	int InitGrid(int numx, int numy, float size);
	void SetGrid(int numx, int numy, float size);
	int Update(Matrix view, Matrix proj);
	int Render();
	void Release();
};

