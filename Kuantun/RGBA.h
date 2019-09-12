#pragma once

#include <SDL.h>
//No hay a
class RGBA{
public:
	float r, g, b;
public:
	RGBA();
	RGBA(float r, float g, float b);
	RGBA	operator + (const RGBA col);
	RGBA	operator - (const RGBA col);
	RGBA	operator * (const float factor);
	RGBA	operator / (const float factor);
	Uint32 toUint32();
	void clamp();
	~RGBA();
};

