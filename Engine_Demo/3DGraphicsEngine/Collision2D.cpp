#include "Collision2D.h"


Collision2D::Collision2D()
	:/* m_Rect(),*/ m_Circle()/*, m_CenterPosX(0), m_CenterPosZ(0)*/
{

}

Collision2D::~Collision2D()
{

}

bool Collision2D::Box_Point_Coll(CollRect rect, Vector3 pos)
{
	if (rect.Bottom <= pos.x && pos.x <= rect.Right
		&& rect.Top <= pos.z && pos.z <= rect.Bottom)
	{
		return true;
	}

	return false;
}

bool Collision2D::Circle_Point_Coll(CollCircle circle, Vector3 pos)
{
	// 점과 원의 중심 사이의 거리 구해주기

	float deltaX = circle.CenterX - pos.x;
	float deltaY = circle.CenterY - pos.z;
	float lenght = sqrt(deltaX * deltaX + deltaY * deltaY);

	if (lenght < circle.Radius)
		return true;

	return false;
}

bool Collision2D::AABBColl(CollRect rect, CollRect tgrect)
{
	if (rect.Left < tgrect.Right &&
		rect.Right > tgrect.Left &&
		rect.Top < tgrect.Bottom &&
		rect.Bottom  > tgrect.Top)
	{
		return true;
	}
	return false;
}


bool Collision2D::CCColl(CollCircle circle1, CollCircle circle2)
{
	float delatX = circle1.CenterX - circle2.CenterX;
	float delatY = circle1.CenterY - circle2.CenterY;
	float lenght = sqrt(delatX * delatX + delatY * delatY);

	if (lenght < (circle1.Radius + circle2.Radius))
		return true;

	return false;
} 

int Collision2D::DrawCollRange(CollCircle circle1)
{
	return OK_SIGN;
}
