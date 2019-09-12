#pragma once
#include "Vector3D.h"

class AABB {
public:
	Vector3D min, max;
public:
	AABB();
	AABB(Vector3D min, Vector3D max);
	bool intersect(Vector3D &rayorig, Vector3D &raydir);
};

