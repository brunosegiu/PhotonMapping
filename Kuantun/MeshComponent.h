#pragma once

#include "Material.h"
#include "Vector3D.h"

class MeshComponent{

public:

	float* vertexBuffer;
	float* normalBuffer;
	float* uvBuffer;
	unsigned int vertexCount;
	
	Material* mat;

	float* cudaVertexBuffer;

public:
	MeshComponent();
	MeshComponent(float* vertexBuffer, float* normalBuffer, float* uvBuffer, Material* mat, unsigned int vertexCount);
	Vector3D* intersect(Vector3D &rayorig, Vector3D &raydir,unsigned int &index, float &u, float &v);
	Vector3D getNormal(unsigned int index, float u, float v, bool smooth);
	void getTextureUV(float tri_u, float tri_v, unsigned int index, float &text_u, float &text_v);
	~MeshComponent();
};

