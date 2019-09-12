#include "MeshComponent.h"

MeshComponent::MeshComponent() {
	this->vertexBuffer = NULL;
	this->normalBuffer = NULL;
	this->uvBuffer = NULL;
	this->mat = new Material();
	this->vertexCount = 0;
}
MeshComponent::MeshComponent(float* vertexBuffer, float* normalBuffer, float* uvBuffer, Material* mat, unsigned int vertexCount) {
	this->vertexBuffer = vertexBuffer;
	this->normalBuffer = normalBuffer;
	this->uvBuffer = uvBuffer;
	this->mat = mat;
	this->vertexCount = vertexCount;
	for (unsigned int i = 0; i < vertexCount; i += 9) {
		vertexBuffer[i + 3] -= vertexBuffer[i];
		vertexBuffer[i + 4] -= vertexBuffer[i + 1];
		vertexBuffer[i + 5] -= vertexBuffer[i + 2];
		vertexBuffer[i + 6] -= vertexBuffer[i];
		vertexBuffer[i + 7] -= vertexBuffer[i + 1];
		vertexBuffer[i + 8] -= vertexBuffer[i + 2];
	}
}
Vector3D* MeshComponent::intersect(Vector3D &rayorig, Vector3D &raydir, unsigned int &index, float &u, float &v) {
	float currDist, tempDist;
	currDist = tempDist = INFINITY;
	Vector3D* hitPoint = NULL;
	for (unsigned int i = 0; i < this->vertexCount; i += 9) {
		Vector3D v0 = Vector3D(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]);
		Vector3D e1 = Vector3D(vertexBuffer[i + 3], vertexBuffer[i + 4], vertexBuffer[i + 5]);
		Vector3D e2 = Vector3D(vertexBuffer[i + 6], vertexBuffer[i + 7], vertexBuffer[i + 8]);
		Vector3D pvec = raydir.GetCross(e2);
		float det = e1.GetDot(pvec);
		if (det >= FLT_EPSILON || det <= -FLT_EPSILON) {
			float inv_det = 1.0f / det;
			Vector3D tvec = rayorig - v0;
			float ut = tvec.GetDot(pvec) * inv_det;
			if (ut >= 0 && ut <= 1) {
				Vector3D qvec = tvec.GetCross(e1);
				float vt = raydir.GetDot(qvec) * inv_det;
				if (vt >= 0 && ut + vt <= 1) {
					tempDist = e2.GetDot(qvec) * inv_det;
					if (tempDist < currDist && tempDist > FLT_EPSILON) {
						currDist = tempDist;
						Vector3D hit = raydir * currDist + rayorig;
						index = i;
						u = ut;
						v = vt;
						if (hitPoint) {
							hitPoint->x = hit.x;
							hitPoint->y = hit.y;
							hitPoint->z = hit.z;
						}
						else {
							hitPoint = new Vector3D(hit.x, hit.y, hit.z);
						}
					}
				}
			}
		}
	}
	return hitPoint;
}

Vector3D MeshComponent::getNormal(unsigned int index, float u, float v, bool smooth) {
	if (this->normalBuffer && smooth) {
		Vector3D n0 = Vector3D(normalBuffer[index], normalBuffer[index + 1], normalBuffer[index + 2]);
		Vector3D n1 = Vector3D(normalBuffer[index + 3], normalBuffer[index + 4], normalBuffer[index + 5]);
		Vector3D n2 = Vector3D(normalBuffer[index + 6], normalBuffer[index + 7], normalBuffer[index + 8]);
		return ( n0 * (1-u-v) + n1 * u + n2 * v).GetNormalized();
	}
	else {
		unsigned int i = index;
		Vector3D e1 = Vector3D(vertexBuffer[i + 3], vertexBuffer[i + 4], vertexBuffer[i + 5]);
		Vector3D e2 = Vector3D(vertexBuffer[i + 6], vertexBuffer[i + 7], vertexBuffer[i + 8]);
		return e1.GetCross(e2).GetNormalized();
	}
}

void MeshComponent::getTextureUV(float tri_u, float tri_v, unsigned int index, float &text_u, float &text_v) {
	if (uvBuffer) {
		index = (index / 3) * 2;
		float t0x = uvBuffer[index]; index++;
		float t0y = uvBuffer[index]; index++;
		float t1x = uvBuffer[index]; index++;
		float t1y = uvBuffer[index]; index++;
		float t2x = uvBuffer[index]; index++;
		float t2y = uvBuffer[index]; index++;
		float tri_w = (1 - tri_u - tri_v);
		text_u = t0x * tri_w + t1x * tri_u + t2x * tri_v;
		text_v = t0y * tri_w + t1y * tri_u + t2y * tri_v;
	}
	else {
		text_u = text_v = -1;
	}
}

MeshComponent::~MeshComponent() {
	delete[] this->vertexBuffer;
	delete[] this->normalBuffer;
	delete[] this->uvBuffer;
	delete mat;
}