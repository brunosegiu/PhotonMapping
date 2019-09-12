#pragma once
#ifndef _POINTDIRLIGHT_
#define _POINTDIRLIgHT_
#include "Light.h"
#include "Vector3D.h"
class PointDirLight:public Light{
public: 
	Vector3D position;
	Vector3D direction;

	PointDirLight(Vector3D position, Vector3D direction, float intensity);
	~PointDirLight();
};
#endif

