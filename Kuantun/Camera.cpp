#include "Camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <stdio.h>

Camera::Camera(){
	this->position = Vector3D();
	this->reference = Vector3D();
	this->up = Vector3D();
	this->fov = 0.0f;
}

Camera::Camera(Vector3D position, Vector3D reference, Vector3D up, float fov) {
	this->position = position;
	this->reference = reference;
	this->up = up;
	this->fov = fov;
}

Vector3D* Camera::generateRayDirs(unsigned int width, unsigned int height) {
	Vector3D* rays = new Vector3D[width * height];
	Vector3D flecha = reference - position;
	Vector3D right = flecha.GetCross(up);
	right = right.GetNormalized();
	float x = 2 * tanf(fov*0.5f*float(M_PI) / 180.0f)*flecha.GetMagnitude();
	float y = ((float)height / width)*x;
	float FacX = (float)x / width;
	float FacY = (float)y / height;
	Vector3D aux = up.GetMul(y*0.5f) + (right.GetMul(-x*0.5f)) + reference;
	for (unsigned int i = 0; i < height; i++){
		for (unsigned int j = 0; j < width; j++){
			Vector3D ret = ((aux + right.GetMul((float)j*FacX) - up.GetMul((float)i*FacY)) - this->position).GetNormalized();
			rays[i*width + j] = ret;
		}
	}
	return rays;
}


Camera::~Camera(){
}
