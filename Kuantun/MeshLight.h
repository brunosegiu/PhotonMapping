#pragma once
#ifndef __MESHLIGHT__
#define __MESHLIGHT__

#include "Light.h"
#include "PointDirLight.h"
#include <string>
#include "Scene.h"
#include "Mesh.h"

class Scene;

class MeshLight : public Mesh{

public:
	MeshLight(std::string path, float intensity, Scene* scene);
	virtual ~MeshLight();

};

#endif