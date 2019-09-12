#pragma once
#include "Vector3D.h"
#include "Light.h"

class DirectionalLight : public Light {
public:
	Vector3D direction;
public:
	DirectionalLight(Vector3D position, float intensity);
	virtual ~DirectionalLight();
};