#include "RGBA.h"
#include <algorithm>

RGBA::RGBA() {
	this->r = this->g = this->b = 0.0f;
}

RGBA::RGBA(float r, float g, float b) {
	this->r = r;
	this->g = g;
	this->b = b;
}

RGBA	RGBA::operator + (const RGBA col) {
	return RGBA(this->r + col.r, this->g + col.g, this->b + col.b);
}

RGBA	RGBA::operator - (const RGBA col) {
	return RGBA(this->r - col.r, this->g - col.g, this->b - col.b);
}

RGBA	RGBA::operator * (const float factor) {
	return RGBA(this->r * factor, this->g * factor, this->b * factor);
}

RGBA	RGBA::operator / (const float factor) {
	return RGBA(this->r / factor, this->g / factor, this->b / factor);
}

Uint32 RGBA::toUint32() {
	Uint32 ret = 0;
	if (this->r > 1.0f)
		this->r = 1.0f;
	if (this->g > 1.0f)
		this->g = 1.0f;
	if (this->b > 1.0f)
		this->b = 1.0f;

	this->r *= 255;
	this->g *= 255;
	this->b *= 255;
	return (0xFF000000 & Uint32(r) << (6 * 4)) | (0x00FF0000 & Uint32(g) << (4 * 4)) | (0x0000FF00 & Uint32(b) << (4 * 2));
}

void RGBA::clamp() {
	if (this->r > 1.0f)
		this->r = 1.0f;
	if (this->g > 1.0f)
		this->g = 1.0f;
	if (this->b > 1.0f)
		this->b = 1.0f;
}

RGBA::~RGBA() {

}