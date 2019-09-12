#pragma once
#include "Vector3D.h"
#include "Light.h"

class PointLight : public Light {
public:
	Vector3D position;
public:
	PointLight(Vector3D position, float intensity);
	virtual ~PointLight();
};