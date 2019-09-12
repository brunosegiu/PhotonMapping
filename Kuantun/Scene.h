#pragma once

#include <vector>

#include "Mesh.h"
#include "MeshLight.h"
#include "Camera.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "AreaLight.h"
#include "Photon.h"
#include "PhotonMap.h"
#include "PhotonsIterator.h"

using namespace std;

class Scene{
public:
	vector<Mesh> objects;
	vector<Light*> lights;
	PhotonMap* photonMap;
	PhotonMap* causticMap;
	unsigned int nPhotons;
	Camera cam;
	RGBA trace(Vector3D &rayOrig, Vector3D &rayDir, unsigned int currentDepth, float currRefrCoef);
	RGBA quickTrace(Vector3D &rayOrig, Vector3D &rayDir);
	RGBA shadow(Vector3D &rayOrig, Vector3D &rayDir, unsigned int currentDepth, Vector3D &normal, Vector3D &inter, Material &mat, MeshComponent* comp,
					float u, float v, unsigned int index, float currRefrCoef, unsigned int modelIndex);
	RGBA quickShadow(Vector3D &rayOrig, Vector3D &rayDir, Vector3D &normal, Vector3D &inter, Material &mat, MeshComponent* comp,
		float u, float v, unsigned int index);
	//por ahora lo dejo en un void
	void calculatePhotonsPerSource(); //Calcula cuantos photones debe emitir cada fuente de luz en la escena
	void propagatePhoton(Photon ph, vector<Photon> &photons, unsigned int depth, float currRefrCoef, bool isCaustic);
	void emitPhotons();
	void emitCausticPhotons();
public:
	Scene(int type);
	Scene(vector<Mesh> objects, vector<Light*> lights, Camera cam);
	RGBA* draw(unsigned int width, unsigned int height, unsigned int supersample);
	RGBA* quickDraw(unsigned int width, unsigned int height);
	void genPhotonMap();
	void save(string path);
	static Scene* load(string path);
	~Scene();
};

