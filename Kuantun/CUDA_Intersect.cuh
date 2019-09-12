#pragma once

#include "Vector3D.h"

#define CUDABLOCKSIZE 128

typedef  struct vertex {
	float x, y, z;
}vertex;

Vector3D* CUDA_Intersect(Vector3D &rayorig, Vector3D &raydir, unsigned int &index, float &u, float &v, float* vertexBuffer, unsigned int vertexCount);
