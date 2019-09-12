#include "Material.h"

#include <algorithm>

Material::Material(){
	this->ambient = RGBA();
	this->diffuse = RGBA();
	this->specular = RGBA();
	this->bitmap = NULL;
	this->textWidth = this->textHeight = 0;
	this->shininess = 0;
	this->diffuseReflection = 0;
}

Material::Material(RGBA ambient, RGBA diffuse, RGBA specular, float shininess, string texturePath, float reflectionIndex, float transmissionIndex, float transparency) {
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess / 5000.0f; //Normalizar
	this->reflectionIndex = reflectionIndex;
	this->transmissionIndex = transmissionIndex;
	this->transparency = transparency;
	this->diffuseReflection = std::max(std::max(diffuse.r, diffuse.g), diffuse.b);
	this->bitmap = NULL;
	if (texturePath != "") {
		FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(texturePath.c_str());
		this->bitmap = FreeImage_Load(fif, texturePath.c_str());
		this->bitmap = FreeImage_ConvertTo24Bits(this->bitmap);
		this->textHeight = FreeImage_GetHeight(bitmap);
		this->textWidth = FreeImage_GetWidth(bitmap);
	}
}

RGBA Material::getTextureColor(float u, float v) {
	int x = int(u * this->textWidth);
	int y = int(v * this->textHeight);
	RGBQUAD color;
	FreeImage_GetPixelColor(bitmap, x, y, &color);
	return RGBA(color.rgbRed / 255.0f, color.rgbGreen / 255.0f, color.rgbBlue / 255.0f);
}

Material::~Material(){
	if (bitmap)
		delete[] bitmap;
}
