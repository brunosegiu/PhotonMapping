#pragma once

#include "Vector3D.h"

class Camera{
public:
	Camera();
	Camera(Vector3D position, Vector3D reference, Vector3D up, float fov);
	Vector3D* generateRayDirs(unsigned int width, unsigned int height);
	~Camera();
public:
	Vector3D position;
	Vector3D reference;
	Vector3D up;
	float fov;
};

