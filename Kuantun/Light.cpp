#include "Light.h"

Light::Light(float intensity) {
	this->intensity = intensity;
	this->nPhotons = 0;
	this->nCausticPhotons = 0;
}

Light::~Light() {
}