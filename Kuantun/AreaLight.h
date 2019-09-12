#pragma once

#include <vector>

#include "Light.h"
#include "Vector3D.h"
#include "PointDirLight.h"
#include "Scene.h"

using namespace std;

class Scene;

class AreaLight : public Light {
public:
	vector<PointDirLight*> points;
public:
	AreaLight(Vector3D v0, Vector3D v1, Vector3D v2, float intensity, Scene* scene);
	virtual ~AreaLight();
};