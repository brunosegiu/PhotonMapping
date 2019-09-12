#pragma once

#include <string>

#include "FreeImage.h"

#include "RGBA.h"

using namespace std;

class Material{
public:
	RGBA ambient, diffuse, specular;
	FIBITMAP* bitmap;
	int textWidth, textHeight;
	float shininess, reflectionIndex, transmissionIndex, transparency, diffuseReflection;
public:
	Material();
	Material(RGBA ambient, RGBA diffuse, RGBA specular, float shininess, string texturePath, float reflectionIndex, float transmissionIndex, float transparency);
	RGBA getTextureColor(float u, float v);
	~Material();
};

