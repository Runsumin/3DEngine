#pragma once
#include "Light.h"
#include "D3DDevice.h"

class DirectionLight : public Light
{
public:
	DirectionLight(Vector3 pos, Vector4 direction, Vector4 diffuse, Vector4 ambient, float range);
	~DirectionLight();
private:

public:
	virtual void InitializeLight() override;
	virtual void UpdateLight(float time) override;

};

