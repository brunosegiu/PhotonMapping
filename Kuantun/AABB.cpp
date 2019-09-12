#include "AABB.h"
#include <algorithm>

AABB::AABB() {

}
AABB::AABB(Vector3D min, Vector3D max) {
	this->min = min;
	this->max = max;
}
bool AABB::intersect(Vector3D &rayorig, Vector3D &raydir) {
	Vector3D invdir = Vector3D(1.0f / raydir.x, 1.0f / raydir.y, 1.0f / raydir.z);
	Vector3D mins = (min - rayorig).GetCoordProduct(invdir);
	Vector3D maxs = (max - rayorig).GetCoordProduct(invdir);
	float tmin = std::max(std::max(std::min(mins.x, maxs.x), std::min(mins.y, maxs.y)), std::min(mins.z, maxs.z));
	float tmax = std::min(std::min(std::max(mins.x, maxs.x), std::max(mins.y, maxs.y)), std::max(mins.z, maxs.z));
	if (tmax < 0 || tmin > tmax){
		return false;
	}
	return true;
}