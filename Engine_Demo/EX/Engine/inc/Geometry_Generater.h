///
///
///
/// 테스트용 기하 도형 생성 클래스
/// [2021/02/09 RUNSUMIN]
#pragma once
#include <SimpleMath.h>
#include "D3DDevice.h"
#include "Shader.h"
#include "EngineInterface.h"
class Geometry_Generater : public GRAPHICENGINE::IGeoMetry_InterFace
{
public:
	struct Vertex
	{
		Vector3 pos;
		Vector4 Color;
	};

	struct Index
	{
		int index;
	};

	/// 상수버퍼 (16바이트 정렬)
	_declspec(align(16)) struct CBGEOMTRY
	{
		Matrix mWorld;
		Matrix mView;
		Matrix mProj;
	};

	ID3D11Device* m_pDevCopy;					// 디바이스 카피
	ID3D11DeviceContext* m_pDevConCopy;			// 디바이스 컨텍스트 카피
	ID3D11Buffer* m_pCbVertex;
	Shader* m_pShader;

	Matrix Pos;
	Matrix Scale;
	Matrix Rot;

	CBGEOMTRY m_cbData;

	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;

	virtual void Initialize() abstract;
	virtual void Update(Matrix World) abstract;
	virtual void Draw() abstract;
	virtual void Release() abstract;
	virtual void ChangeColor(Vector4 color) abstract;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Box : public Geometry_Generater
{
public:
	Box(float width, float height, float depth, Vector4 color);
	~Box();

	float m_Width;		// X
	float m_Height;		// Y
	float m_Depth;		// Z
	Vector4 m_Color;
	Vertex m_Box;

	virtual void Initialize() override;
	virtual void Update(Matrix World) override;
	virtual void Draw() override;
	virtual void Release() override;
	virtual void ChangeColor(Vector4 color) override;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Sphere : public Geometry_Generater
{
public:
	Sphere(float radious, int slicecount, int stackcount, Vector4 color);
	~Sphere();

	vector<Vertex> m_Sphere;
	vector<int> m_index;
	float m_Radious;
	int m_SliceCount;
	int m_StackCount;
	Vector4 m_Color;


	virtual void Initialize() override;
	virtual void Update(Matrix World) override;
	virtual void Draw() override;
	virtual void Release() override;
	virtual void ChangeColor(Vector4 color) override;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Line : public Geometry_Generater
{
public:
	Line(float startpos, float endpos, float thickness, Vector4 color);
	~Line();

	vector<Vertex> m_Line;
	float m_Startpos;
	float m_Endpos;
	float m_Thickness;

public:
	virtual void Initialize() override;
	virtual void Update(Matrix World) override;
	virtual void Draw() override;
	virtual void Release() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class Cylinder : public Geometry_Generater
{
public:
	Cylinder();
	~Cylinder();

	Vertex m_Cylinder;

	void Initialize(float size, Vector4 Color);
	void Update();
	void Draw();
};

