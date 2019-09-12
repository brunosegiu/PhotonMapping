#include "PointDirLight.h"



PointDirLight::PointDirLight(Vector3D position, Vector3D direction, float intensity) : Light(intensity) {
	this->position = position;
	this->direction = direction;
}


PointDirLight::~PointDirLight()
{
}
