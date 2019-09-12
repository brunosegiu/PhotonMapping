#pragma once

class Light{
public:
	Light(float intensity);
	virtual ~Light();
public:
	float intensity;
	unsigned int nPhotons;
	unsigned int nCausticPhotons;
};