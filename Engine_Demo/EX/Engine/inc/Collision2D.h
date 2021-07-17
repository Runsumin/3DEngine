//
//
//
// 점프가 없기 때문에 
// [2020/08/19 RUNSUMIN]
#pragma once
#include "DxDefine.h"
struct CollRect
{
	float Left, Top, Right, Bottom;
};

struct CollCircle
{
	float CenterX, CenterY, Radius;
};

class Collision2D
{
public:
	Collision2D();
	~Collision2D();

	//CollRect   m_Rect;
	CollCircle m_Circle;
	//float m_CenterPosX;
	//float m_CenterPosZ;

public:
	bool Box_Point_Coll(CollRect rect, Vector3 pos);
	bool Circle_Point_Coll(CollCircle circle, Vector3 pos);
	bool AABBColl(CollRect rect, CollRect tgrect);
	bool CCColl(CollCircle circle1, CollCircle circle2);
	int DrawCollRange(CollCircle circle1);
};

