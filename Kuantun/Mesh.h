#pragma once

#include <vector>
#include <string>

#include "MeshComponent.h"
#include "Vector3D.h"
#include "AABB.h"

using namespace std;

class Mesh {
public:
	vector<MeshComponent*> parts;
	AABB boundingbox;
	Vector3D acumTranslate;
	string path;
public:
	Mesh(string path);
	Vector3D* intersect(Vector3D &rayorig, Vector3D & raydir, MeshComponent* &curr, unsigned int &index, float &u, float &v);
	void translate(Vector3D d);
	~Mesh();
};