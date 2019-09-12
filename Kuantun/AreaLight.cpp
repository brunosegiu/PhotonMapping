#include "AreaLight.h"
#include <cstdlib>
#include "Photon.h"
#include <random>
AreaLight::AreaLight(Vector3D v0, Vector3D v1, Vector3D v2, float intensity, Scene* scene) :Light(intensity) {

	Vector3D edge1 = v1.GetSub(v0);
	Vector3D edge2 = v2.GetSub(v0);
	Vector3D normal = edge1.GetCross(edge2).GetNormalized();

	float size1 = edge1.GetMagnitude();
	float size2 = edge2.GetMagnitude();
	float intensidad2 = this->intensity / (size1 * size2 * 25);
	edge1 = edge1.GetNormalized();
	edge2 = edge2.GetNormalized();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int>dist(0, 255);

	int rand1;
	int rand2;

	for (float i = 0.0f; i < size1; i += 0.2f) {
		for (float j = 0.0f; j < size2; j += 0.2f) {
			rand1 = dist(gen);
			rand2 = dist(gen) % 128;
			Vector3D position = v0 + edge1.GetMul(i) + edge2.GetMul(j);
			Vector3D direction = Vector3D(normal.x + Photon::COS_PHI[rand1] * Photon::SIN_THETA[rand2], normal.y + Photon::SIN_PHI[rand1] * Photon::SIN_THETA[rand2], //direccion
				normal.z + Photon::COS_THETA[rand2]).GetNormalized();
			this->points.push_back(new PointDirLight(position, direction, intensidad2));
		}
	}

	scene->lights.insert(scene->lights.end(), points.begin(), points.end());
}

AreaLight::~AreaLight() {

}