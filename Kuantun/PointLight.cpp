#include "PointLight.h"

PointLight::PointLight(Vector3D position, float intensity):Light(intensity) {
	this->position = position;
}

PointLight::~PointLight() {}