#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Vector3D direction, float intensity) :Light(intensity) {
	this->direction = direction.GetNormalized();
}

DirectionalLight::~DirectionalLight() {}