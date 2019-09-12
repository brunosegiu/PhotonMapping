#include "Scene.h"
#include <algorithm>
#include <random>
#include <cstdlib>
#include <stdlib.h>
#include <functional>
#include <chrono>
#include <math.h>
#include "tinyxml2.h"
#include <fstream>

#include "PhotonMap.h"
#include "PhotonsIterator.h"

#define MAX_DEPTH 5;
#define CANT_PHOTONS 100000; //Numero de fotones total a disparar
#define CANT_CAUSTIC_PHOTONS 100000; //Numero de fotones para el mapa de causticas

char russianRoullete(Material &m) {
	if (m.reflectionIndex == 1) return '1';
	else if (m.diffuseReflection == 1) return '0';
	else {
		float ra = unsigned int(rand()) / float(RAND_MAX);
		if (ra <= m.diffuseReflection) {
			return '0';
		}
		else if (ra <= m.reflectionIndex + m.diffuseReflection) {
			return '1';
		}
		else {
			return '2';
		}
	}

}

Scene::Scene(int type){
	if (type == 1) {
		this->objects.push_back(Mesh("assets/models/basic/skybox")); //NO BORRAR NI MODIFICAR ESTA LINEA
		this->objects.push_back(Mesh("assets/models/basic/teapot"));
		this->lights.push_back(new PointLight(Vector3D(0.0f, 8.0, -1.0), 100.0f));
		this->lights.push_back(new DirectionalLight(Vector3D(0.0f, 0.0, -1.0), 0.3f));
		this->cam = Camera(Vector3D(0.0f, 6.0f, 15.0f), Vector3D(0.0f, 6.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f), 45.0f);
	}else if (type == 2){
		this->objects.push_back(Mesh("assets/models/basic/skybox")); //NO BORRAR NI MODIFICAR ESTA LINEA
		this->objects.push_back(Mesh("assets/models/basic/teapot"));
		this->objects.push_back(Mesh("assets/models/basic/table"));
		this->objects.push_back(Mesh("assets/models/basic/dragon"));
		this->objects.push_back(Mesh("assets/models/basic/stormtrooper"));
		this->objects.push_back(Mesh("assets/models/basic/tesla"));
		this->objects.push_back(Mesh("assets/models/basic/plane"));
		this->lights.push_back(new PointLight(Vector3D(0.0f, 8.0, -1.0), 10.0f));
		this->lights.push_back(new DirectionalLight(Vector3D(0.0f, 0.0, -1.0), 0.1f));
		AreaLight(Vector3D(-2.710680, 8.027324, -2.430594), Vector3D(3.159981, 9.248735, 1.090497), Vector3D(-1.624654, 9.188608, 2.222286), 100.0f, this);
		this->cam = Camera(Vector3D(0.0f, 8.0f, 15.0f), Vector3D(0.0f, 6.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f), 70.0f);
	}
	else if (type == 3) {
		this->objects.push_back(Mesh("assets/models/sw/skybox")); //NO BORRAR NI MODIFICAR ESTA LINEA
		this->objects.push_back(Mesh("assets/models/sw/nebulon"));
		this->objects.push_back(Mesh("assets/models/sw/tie"));
		this->objects.push_back(Mesh("assets/models/sw/dest"));
		this->objects.push_back(Mesh("assets/models/sw/falcon"));
		this->lights.push_back(new DirectionalLight(Vector3D(1.0f, 0.0, 1.0), 0.3f));
		this->lights.push_back(new DirectionalLight(Vector3D(-1.0f, 0.0, 0.0), 0.4f));
		this->lights.push_back(new PointLight(Vector3D(0.0f, 10.0, 0.0), 1000.0f));
		this->cam = Camera(Vector3D(-100.0f, 0.0f, 0.0f), Vector3D(-200.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f), 120.0f);
	}
	
}

void Scene::genPhotonMap() {
	calculatePhotonsPerSource(); //Asigno la cantidad de fotones que le corresponde a cada luz
	emitPhotons();
	emitCausticPhotons();
}

RGBA* Scene::draw(unsigned int width, unsigned int height, unsigned int supersample) {
	RGBA* screen = new RGBA[width * height];
	Vector3D* rayDirs = this->cam.generateRayDirs(width*supersample, height*supersample);
	Vector3D rayOrig = this->cam.position;
	#pragma omp parallel for 
	for (int i = 0; i < int(height); i++) {
		#pragma omp parallel for 
		for (int j = 0; j < int(width); j++) {
			RGBA color = RGBA(0.0f, 0.0f, 0.0f);
			for (unsigned int k = 0; k < supersample; k++) {
				for (unsigned int l = 0; l < supersample; l++) {
					color = color + trace(rayOrig, rayDirs[ (i*width*supersample + k + j) * supersample + l], 0, 1.0);
				}
			}
			screen[i*width + j] = (color * (1.0f / (supersample * supersample)));
		}
	}
	delete[] rayDirs;
	return screen;
}

RGBA* Scene::quickDraw(unsigned int width, unsigned int height) {
	RGBA* screen = new RGBA[width * height];
	Vector3D* rayDirs = this->cam.generateRayDirs(width, height);
	Vector3D rayOrig = this->cam.position;
	#pragma omp parallel for 
	for (int i = 0; i < int(height); i++) {
		#pragma omp parallel for 
		for (int j = 0; j < int(width); j++) {
			screen[i*width + j] = quickTrace(rayOrig, rayDirs[i*width + j]);
		}
	}
	delete[] rayDirs;
	return screen;
}

RGBA Scene::quickTrace(Vector3D &rayOrig, Vector3D &rayDir) {
	Vector3D* tempInter, *inter;
	tempInter = inter = NULL;
	float tempDist, currentDist;
	currentDist = tempDist = INFINITY;
	MeshComponent* tempComp, *comp;
	tempComp = comp = NULL;
	unsigned int index = 0;
	unsigned int tempindex = 0;
	float ut, vt, u, v;
	for (unsigned int i = 0; i < this->objects.size(); i++) {
		if (this->objects[i].boundingbox.intersect(rayOrig, rayDir)) {
			tempInter = this->objects[i].intersect(rayOrig, rayDir, tempComp, tempindex, ut, vt);
			if (tempInter) {
				tempDist = tempInter->GetDistance(rayOrig);
				if (tempDist < currentDist) {
					u = ut;
					v = vt;
					currentDist = tempDist;
					comp = tempComp;
					if (inter) delete inter;
					inter = tempInter;
					index = tempindex;
				}
				else {
					delete tempInter;
				}
			}
		}
	}
	if (inter) {
		Vector3D normalOnPoint = comp->getNormal(index, u, v, true);
		RGBA color = quickShadow(rayOrig, rayDir,normalOnPoint, *inter, *comp->mat, comp, u, v, index);
		delete inter;
		return color;
	}
	else {
		return RGBA(0, 0, 0);
	}
}

RGBA Scene::quickShadow(Vector3D &rayOrig, Vector3D &rayDir, Vector3D &normal, Vector3D &inter, Material &mat, MeshComponent* comp, float u, float v, unsigned int index) {
	RGBA color;
	color = mat.ambient;
	for (unsigned int i = 0; i < lights.size(); i++) {
		MeshComponent* comp_trash;
		unsigned int k_trash;
		float u_trash, v_trash;
		DirectionalLight* directional = dynamic_cast<DirectionalLight*>(lights[i]);
		PointLight* point = dynamic_cast<PointLight*>(lights[i]);
		PointDirLight* areaPoint = dynamic_cast<PointDirLight*>(lights[i]);
		Vector3D lDir;
		float distance = -1;
		if (directional) {
			lDir = -directional->direction;
		}
		else if (point) {
			lDir = point->position - inter;
			distance = lDir.GetMagnitude();
			lDir = lDir / distance;
		}
		else if (areaPoint) {
			lDir = -areaPoint->direction;
			distance = areaPoint->position.GetDistance(inter);
		}
		float factor = normal.GetDot(lDir);
		if (factor > 0) {
			factor = factor*lights[i]->intensity;
			if (point || areaPoint) {
				factor = factor / (distance*distance);
			}
			if (factor>1.0f)
				factor = 1.0f;
			if (mat.bitmap) {
				float ut, vt;
				comp->getTextureUV(u, v, index, ut, vt);
				color = color + mat.getTextureColor(ut, vt) * factor;
			}
			else {
				color = color + mat.diffuse * factor;
			}
		}
	}
	return color;
}


RGBA Scene::trace(Vector3D &rayOrig, Vector3D &rayDir, unsigned int currentDepth, float currRefrCoef){
	Vector3D* tempInter, *inter;
	tempInter = inter = NULL;
	float tempDist, currentDist;
	currentDist = tempDist = INFINITY;
	MeshComponent* tempComp,* comp;
	tempComp = comp = NULL;
	unsigned int index = 0;
	unsigned int tempindex = 0;
	float ut, vt, u, v;
	unsigned int obji = 0;
	if (currentDepth < 5) {
		for (unsigned int i = 0; i < this->objects.size(); i++) {
			if (this->objects[i].boundingbox.intersect(rayOrig, rayDir)) {
				tempInter = this->objects[i].intersect(rayOrig, rayDir, tempComp, tempindex, ut, vt);
				if (tempInter) {
					tempDist = tempInter->GetDistance(rayOrig);
					if (tempDist < currentDist) {
						u = ut;
						v = vt;
						currentDist = tempDist;
						comp = tempComp;
						if (inter) delete inter;
						inter = tempInter;
						index = tempindex;
						obji = i;
					}
					else {
						delete tempInter;
					}
				}
			}
		}
	}
	if (inter) {
		Vector3D normalOnPoint = comp->getNormal(index, u, v, true);
		RGBA color = shadow(rayOrig, rayDir, currentDepth, normalOnPoint, *inter, *comp->mat, comp, u, v, index, currRefrCoef, obji);
		delete inter;
		return color;
	}
	else {
		return RGBA(0,0,0);
	}
}

RGBA Scene::shadow(Vector3D &rayOrig, Vector3D &rayDir, unsigned int currentDepth, Vector3D &normal, Vector3D &inter, Material &mat, MeshComponent* comp, float u, float v, unsigned int index, float currRefrCoef, unsigned int modelIndex) {
	RGBA color;
	color = mat.ambient;
	for (unsigned int i = 0; i < lights.size(); i++) {
		MeshComponent* comp_trash;
		unsigned int k_trash;
		float u_trash, v_trash;
		DirectionalLight* directional = dynamic_cast<DirectionalLight*>(lights[i]);
		PointLight* point = dynamic_cast<PointLight*>(lights[i]);
		PointDirLight* areaPoint = dynamic_cast<PointDirLight*>(lights[i]);
		Vector3D lDir;
		float distance = -1;
		if (directional) {
			lDir = -directional->direction;
		}else if (point){
			lDir = point->position - inter;
			distance = lDir.GetMagnitude();
			lDir = lDir / distance;
		}else if(areaPoint){
			lDir = -areaPoint->direction;
			distance = areaPoint->position.GetDistance(inter);
		}
		float factor = normal.GetDot(lDir);
		if (factor > 0) {
			Vector3D biasedInter = inter + lDir * 0.01f;
			#pragma omp parallel for
			for (int j = 0; j < objects.size(); j++) {
				if (objects[j].boundingbox.intersect(biasedInter, lDir)) {
					Vector3D* lightHit = objects[j].intersect(biasedInter, lDir, comp_trash, k_trash, u_trash, v_trash);
					if (lightHit && (((point || areaPoint) && inter.GetDistance(*lightHit) < distance) || (directional && j > 0))){
						#pragma omp atomic
						factor *= comp_trash->mat->transparency;
					}
					if (lightHit) delete lightHit;
				}
			}
			factor = factor*lights[i]->intensity;
			if (point || areaPoint) {
				factor = factor / (distance*distance);
			}
			if (factor>1.0f)
				factor = 1.0f;
			if (mat.bitmap) {
				float ut, vt;
				comp->getTextureUV(u, v, index, ut, vt);
				color = color + mat.getTextureColor(ut, vt) * factor;
			}
			else {
				color = color + mat.diffuse * factor;
			}
			//Phong
			Vector3D R = lDir.GetSymmetrical(normal).GetNormalized();
			factor *= pow(R.GetDot(rayDir), 10);
			if (factor > 0) {
				RGBA shininess = mat.specular * std::min(1.0f, factor);
				color = color + shininess;
			}
		}
	}
	if (currentDepth < 5) {
		if (mat.reflectionIndex > 0) {
			Vector3D reflDir = (-rayDir).GetSymmetrical(normal).GetNormalized();
			Vector3D reflOrig = inter + normal * 0.01f;
			RGBA speccol = trace(reflOrig, reflDir, currentDepth + 1, currRefrCoef)*mat.reflectionIndex;
			color = color + speccol;
		}

		float transparency = mat.transparency;
		if (transparency > 0.0f) {
			float refrCoef = mat.transmissionIndex;
			float refr_inc = currRefrCoef;
			Vector3D aux_normal = normal;
			if (refrCoef == currRefrCoef) {
				refrCoef = 1.0f;
			}
			if (rayDir.GetDot(normal) >= 1e-4) {
				aux_normal.SetInverted();
			}

			float cos_inc = (rayDir).GetDot(normal);
			if (cos_inc < 1e-4) {
				cos_inc = -cos_inc;
			}
			else {
				std::swap(refr_inc, refrCoef);
			}

			float n = refr_inc / refrCoef;

			float inc_ang = acosf(cos_inc);

			float critical_angle = asinf(n);
			if (fabs(inc_ang - critical_angle) > 1e-4) {
				Vector3D refr_dir = (rayDir*n + aux_normal * (n * cos_inc - sqrtf(1 - n*n*(1 - cos_inc*cos_inc)))).GetNormalized();
				Vector3D refr_orig = inter + refr_dir * 0.1f;
				color.clamp();
				RGBA refrcol = trace(refr_orig, refr_dir, currentDepth + 1, refrCoef);
				color = color * (1.0f - transparency) + refrcol * transparency;
			}
		}
	}
	RGBA ind = RGBA();
	if (this->photonMap->getIrradiance(inter, &normal, 1.5f, 1500, ind, 8)) {
		color = color + ind;
	}
	if (this->causticMap->getIrradiance(inter, &normal, 0.2f, 7500, ind, 1000)) {
		color = color + ind;
	}
	return color;
}

void Scene::calculatePhotonsPerSource() {
	float totalIntensity = 0;
	float intensityFraction = 0;
	for (unsigned int i = 0; i < lights.size(); i++) {
		totalIntensity = totalIntensity + lights[i]->intensity;
	}
	for (unsigned int i = 0; i < lights.size(); i++) {
		intensityFraction = lights[i]->intensity/totalIntensity;
		lights[i]->nPhotons = intensityFraction*CANT_PHOTONS;
		lights[i]->nCausticPhotons = CANT_CAUSTIC_PHOTONS;
	}

}

void Scene::propagatePhoton(Photon ph, vector<Photon> &photons, unsigned int depth, float currRefrCoef, bool isCaustic){
	Vector3D* tempInter, *inter;
	tempInter = inter = NULL;
	float tempDist, currentDist;
	currentDist = tempDist = INFINITY;
	MeshComponent* tempComp, *comp;
	tempComp = comp = NULL;
	unsigned int index = 0;
	unsigned int tempindex = 0;
	float ut, vt, u, v;
	if (depth < 10) {
		for (unsigned int i = 0; i < this->objects.size(); i++) {
			if (this->objects[i].boundingbox.intersect(ph.getLocation(), ph.getDirection())) {
				tempInter = this->objects[i].intersect(ph.getLocation(), ph.getDirection(), tempComp, tempindex, ut, vt);
				if (tempInter) {
					tempDist = tempInter->GetDistance(ph.getLocation());
					if (tempDist < currentDist) {
						u = ut;
						v = vt;
						currentDist = tempDist;
						comp = tempComp;
						if (inter) delete inter;
						inter = tempInter;
						index = tempindex;
					}
				}
			}
		}
		if (inter) {
			if (comp->mat->transparency > 0) {			//se refracta, no considero la reflexión

				Material mat = *comp->mat; 
				float refrCoef = mat.transmissionIndex;
				float refr_inc = currRefrCoef;
				Vector3D normal = comp->getNormal(index, u, v, true);
				Vector3D aux_normal = normal;
				if (refrCoef == currRefrCoef) {
					refrCoef = 1.0f;
				}
				if (ph.getDirection().GetDot(normal) >= 1e-4) {
					aux_normal.SetInverted();
				}

				float cos_inc = (ph.getDirection()).GetDot(normal);
				if (cos_inc < 1e-4) {
					cos_inc = -cos_inc;
				}
				else {
					std::swap(refr_inc, refrCoef);
				}

				float n = refr_inc / refrCoef;

				float inc_ang = acosf(cos_inc);

				float critical_angle = asinf(n);
				if (fabs(inc_ang - critical_angle) > 1e-4) {
					Vector3D refr_dir = (ph.getDirection()*n + aux_normal * (n * cos_inc - sqrtf(1 - n*n*(1 - cos_inc*cos_inc)))).GetNormalized();
					Vector3D refr_orig = *inter + refr_dir * 0.1f;
					RGBA color, phColor;
					if (comp->mat->bitmap) {
						float ut, vt;
						comp->getTextureUV(u, v, index, ut, vt);
						color = comp->mat->getTextureColor(ut, vt);
					}
					else {
						color = comp->mat->diffuse;
					}
					RGBA refrcol;
					refrcol.r = mat.transparency*color.r*ph.getPower().r;
					refrcol.g = mat.transparency*color.g*ph.getPower().g;
					refrcol.b = mat.transparency*color.b*ph.getPower().b;
					propagatePhoton(Photon(refr_orig + refr_dir * 0.1f, refr_dir, refrcol), photons, depth+1, refrCoef, isCaustic);
					
				}
			}
			else {
				RGBA color, phColor;
				if (comp->mat->bitmap) {
					float ut, vt;
					comp->getTextureUV(u, v, index, ut, vt);
					color = comp->mat->getTextureColor(ut, vt);
				}
				else {
					color = comp->mat->diffuse;
				}
				phColor = ph.getPower();
				color.r = color.r*phColor.r; //determino color de la huella
				color.g = color.g*phColor.g;
				color.b = color.b*phColor.b;
				Vector3D bounceDir = (-ph.getDirection()).GetSymmetrical(comp->getNormal(index, u, v, true)).GetNormalized(); //dirección en la q sale de la superficie
				Photon footPrint(*inter, bounceDir, color); //creo la huella

				char r = russianRoullete(*comp->mat);
				if (color.r > 1e-8 || color.g > 1e-8 || color.b > 1e-8) {
					if ((r == '0' || r == '2') && depth > 0) {	//se gurada la huella				 
						#pragma omp critical
						photons.push_back(footPrint);
					}
					if (depth < 10) {				//si no se alcanzó el límite de la recursión
						Vector3D biasedInter = *inter + bounceDir *0.05f;
						if (r == '0') {			//si se refleja de forma difusa
							if(!isCaustic)
							propagatePhoton(Photon(biasedInter, bounceDir, color), photons, depth + 1, currRefrCoef, isCaustic);		//tiro fotón con las mismas carateristicas q la huella;
						}
						else if (r == '1') {	//si se refleja de forma especular
							propagatePhoton(Photon(biasedInter, bounceDir, ph.getPower()), photons, depth + 1, currRefrCoef, isCaustic); //el fotón reflejado mantiene su potencia
						}
					}
				}
			}
		}
	}
}

void Scene::emitPhotons(){
	vector<Photon> origPhotons;
	vector<Photon> postaPhotons;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float>dist(-1.0f, 1.0f);
	std::uniform_int_distribution<int>dist2(0, 255);


	for (unsigned int i = 0; i < lights.size(); i++) {
		PointLight* light = dynamic_cast<PointLight*>((lights[i]));
		PointDirLight* areaLight = dynamic_cast<PointDirLight*>(lights[i]);
		if (light) {
			float photonEnergy = light->intensity / light->nPhotons;
			float x, y, z = 1.0f;
			Vector3D* auxdir= new Vector3D(light->position);
			for (unsigned int emitedPhotons = 0; emitedPhotons < light->nPhotons; emitedPhotons++) {
				bool end = false;
				while (!end) {
					x = dist(gen);
					y = dist(gen);
					z = dist(gen);
					end = (x*x + y*y + z*z) > 1;
				}
				Vector3D dir = Vector3D(x, y, z);
				Vector3D pos = light->position;
				RGBA color = RGBA(photonEnergy, photonEnergy, photonEnergy);
				origPhotons.push_back(Photon(pos, dir, color));
			}
			delete auxdir;
		}else if (areaLight) {
				float photonEnergy = areaLight->intensity / areaLight->nPhotons;
				float x, y, z = 1.0f;
				Vector3D* auxdir = new Vector3D(areaLight->direction);
				for (unsigned int j = 0; j < areaLight->nPhotons; j++) {
					int rand1 = dist2(gen);
					int rand2 = dist2(gen) % 128;
					Vector3D dir = Vector3D(auxdir->x + Photon::COS_PHI[rand1] * Photon::SIN_THETA[rand2], auxdir->y + Photon::SIN_PHI[rand1] * Photon::SIN_THETA[rand2], //direccion
						auxdir->z + Photon::COS_THETA[rand2]).GetNormalized();
					RGBA color = RGBA(photonEnergy, photonEnergy, photonEnergy);
					origPhotons.push_back(Photon(areaLight->position, dir, color));
				}
				delete auxdir;
		}
	}//endfor lights
	#pragma omp parallel for
	for (int j = 0; j < int(origPhotons.size()); j++) {
		propagatePhoton(origPhotons[j], postaPhotons, 0, 1.0,false);
	}
	
	//tengo que crear un const
	const vector<Photon> copia = vector<Photon>(postaPhotons);
	this->photonMap = new PhotonMap(copia);
}

void Scene::emitCausticPhotons() {
	vector<Photon> causticPhotons;
	vector<Photon> postaPhotons;
	vector<Mesh> transpObjects;
	for (unsigned int j = 0; j < objects.size(); j++) {
		for (unsigned int k = 0; k < objects[j].parts.size(); k++) {
			if (objects[j].parts[k]->mat->transparency > 0) {
				transpObjects.push_back(objects[j]);
				break;
			}
		}
	}
	if (transpObjects.size() > 0) {//si vector no es vacio
		for (unsigned int i = 0; i < lights.size(); i++) { 
			PointLight* light = dynamic_cast<PointLight*>((lights[i]));
			if (light) {
				float photonsPerObj = light->nCausticPhotons / transpObjects.size();
				float photonEnergy = light->intensity / light->nCausticPhotons;
				for (unsigned int j = 0; j < transpObjects.size(); j++) {
					Vector3D min = transpObjects[j].boundingbox.min;
					Vector3D max = transpObjects[j].boundingbox.max;
					min.print();
					max.print();
					vector<Vector3D> vertices;
					vertices.push_back((min-light->position));
					vertices.push_back((max-light->position));
					vertices.push_back((Vector3D(max.x, min.y, min.z)-light->position));
					vertices.push_back((Vector3D(min.x, max.y, min.z)-light->position));
					vertices.push_back((Vector3D(min.x, min.y, max.z)-light->position));
					vertices.push_back((Vector3D(max.x, max.y, min.z)-light->position));
					vertices.push_back((Vector3D(min.x, max.y, max.z)-light->position));
					vertices.push_back((Vector3D(max.x, min.y, max.z)-light->position));
					float maxPhi, minPhi, maxTheta, minTheta;
					for (unsigned int k = 0; k < vertices.size(); k++) {
						float theta;
						if (vertices[k].z > 0) theta = atan(sqrtf((pow(vertices[k].x, 2) + pow(vertices[k].y, 2))) / vertices[k].z);
						else if(vertices[k].z < 0) theta = atan(sqrtf((pow(vertices[k].x, 2) + pow(vertices[k].y, 2))) / vertices[k].z)+M_PI;
						else theta = M_PI / 2;
						float phi;
						if (vertices[k].x == 0) {
							if (vertices[k].y >= 0) phi = (M_PI / 2);
							else phi = -(M_PI / 2);
						}
						else if (vertices[k].x > 0) {
							if (vertices[k].y >= 0) phi = atan(vertices[k].y / vertices[k].x);
							else phi = atan(vertices[k].y / vertices[k].x) + (2 * M_PI);
						}
						else phi = atan(vertices[k].y / vertices[k].x) + M_PI;
						
						if (k == 0) { maxTheta = theta; minTheta = theta; maxPhi = phi; minPhi = phi; }
						else {
							maxTheta = std::max(theta, maxTheta);
							maxPhi = std::max(phi, maxPhi);
							minTheta = std::min(theta, minTheta);
							minPhi = std::min(phi, minPhi);
						}
					}
					float solid = (maxPhi - minPhi)*(cos(minTheta) - cos(maxTheta));
					photonEnergy = photonEnergy*(solid / (4 * M_PI));
					for (unsigned int k = 0; k < photonsPerObj; k++) {
						bool inter = false;
						float randTheta, randPhi;
						Vector3D dir;
						MeshComponent* basura;
						unsigned int basurai;
						float basurau, basurav;
						while (!inter) {
							randTheta = minTheta + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxTheta - minTheta)));
							randPhi = minPhi + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxPhi - minPhi)));
							dir = Vector3D(sin(randTheta)*cos(randPhi), sin(randTheta)*sin(randPhi), cos(randTheta)).GetNormalized();
							if (transpObjects[j].intersect(light->position, dir, basura, basurai, basurau, basurav)) inter = true;
						}
						causticPhotons.push_back(Photon(light->position, dir, RGBA(photonEnergy, photonEnergy, photonEnergy)));

					}
				}
			}
		}

	}
	#pragma omp parallel for
	for (int j = 0; j < causticPhotons.size(); j++) {
		propagatePhoton(causticPhotons[j], postaPhotons, 0, 1.0, true);
	}
	const vector<Photon> copia = vector<Photon>(postaPhotons);
	this->causticMap = new PhotonMap(copia);
}



Scene::~Scene(){
	for (unsigned int i = 0; i < lights.size(); i++) {
		delete lights[i];
	}
	if (photonMap) delete photonMap;
	if (causticMap) delete causticMap;
}

void Scene::save(string path) {
	using namespace tinyxml2;
	XMLDocument xmlDoc;

	XMLNode * pRoot = xmlDoc.NewElement("root");
	xmlDoc.InsertFirstChild(pRoot);

	XMLElement* pcam = xmlDoc.NewElement("camera");
	Vector3D position = this->cam.position;
	pcam->SetAttribute("posx", position.x);
	pcam->SetAttribute("posy", position.y);
	pcam->SetAttribute("posz", position.z);

	Vector3D refer = this->cam.reference;
	pcam->SetAttribute("rx", refer.x);
	pcam->SetAttribute("ry", refer.y);
	pcam->SetAttribute("rz", refer.z);

	Vector3D up = this->cam.up;
	pcam->SetAttribute("upx", up.x);
	pcam->SetAttribute("upy", up.y);
	pcam->SetAttribute("upz", up.z);

	pcam->SetAttribute("fov", cam.fov);
	pRoot->InsertEndChild(pcam);

	XMLElement * pobjs = xmlDoc.NewElement("objs");
	for (unsigned int i = 0; i < this->objects.size(); i++) {
		XMLElement * pobj = xmlDoc.NewElement("item");
		pobj->SetAttribute("path", objects[i].path.c_str());
		pobj->SetAttribute("trsx", objects[i].acumTranslate.x);
		pobj->SetAttribute("trsy", objects[i].acumTranslate.y);
		pobj->SetAttribute("trsz", objects[i].acumTranslate.z);
		pobjs->InsertEndChild(pobj);
	}
	pRoot->InsertEndChild(pobjs);

	XMLElement * plights = xmlDoc.NewElement("lights");
	for (unsigned int i = 0; i < this->lights.size(); i++) {
		DirectionalLight* directional = dynamic_cast<DirectionalLight*>(lights[i]);
		PointLight* point = dynamic_cast<PointLight*>(lights[i]);
		XMLElement * plight = xmlDoc.NewElement("item");
		plight->SetAttribute("int", lights[i]->intensity);
		if (point) {
			plight->SetAttribute("type", "point");
			plight->SetAttribute("x", point->position.x);
			plight->SetAttribute("y", point->position.y);
			plight->SetAttribute("z", point->position.z);
			plights->InsertEndChild(plight);
		}
		else if (directional) {
			plight->SetAttribute("type", "dir");
			plight->SetAttribute("x", directional->direction.x);
			plight->SetAttribute("y", directional->direction.y);
			plight->SetAttribute("z", directional->direction.z);
			plights->InsertEndChild(plight);
		}
	}
	pRoot->InsertEndChild(plights);
	xmlDoc.SaveFile(path.c_str());

	if (this->photonMap && this->photonMap->iterator()) {
		ofstream photonMap;
		photonMap.open((path + ".photonMap.csv").c_str());
		photonMap << "Mapa de luz indirecta. \n";
		PhotonMap* mapa = this->photonMap;
		PhotonsIterator* iter = mapa->iterator();
		Photon* ph;
		while (iter->hasNext()) {
			ph = iter->next();
			photonMap << "Position: " << ph->getLocation().x << ", " << ph->getLocation().y << ", " << ph->getLocation().z << "\n";
			photonMap << "Direction: " << ph->getDirection().x << ", " << ph->getDirection().y << ", " << ph->getDirection().z << "\n";
			photonMap << "Color: " << ph->getPower().r << ", " << ph->getPower().g << ", " << ph->getPower().b << "\n";
		}
		photonMap.close();
	}

	if (this->causticMap && this->causticMap->iterator()) {
		ofstream causticMap;
		causticMap.open((path + ".photonMap.csv").c_str());
		causticMap << "Mapa de cáusticas. \n";
		PhotonMap* mapa = this->photonMap;
		PhotonsIterator* iter = mapa->iterator();
		Photon* ph;
		while (iter->hasNext()) {
			ph = iter->next();
			causticMap << "Position: " << ph->getLocation().x << ", " << ph->getLocation().y << ", " << ph->getLocation().z << "\n";
			causticMap << "Direction: " << ph->getDirection().x << ", " << ph->getDirection().y << ", " << ph->getDirection().z << "\n";
			causticMap << "Color: " << ph->getPower().r << ", " << ph->getPower().g << ", " << ph->getPower().b << "\n";
		}
		causticMap.close();
	}
}

Scene* Scene::load(string path) {
	using namespace tinyxml2;
	XMLDocument xmlDoc;
	xmlDoc.LoadFile(path.c_str());

	XMLNode * pRoot = xmlDoc.FirstChild();

	XMLElement* pcam = pRoot->FirstChildElement("camera");
	float x, y, z;
	pcam->QueryFloatAttribute("posx", &x);
	pcam->QueryFloatAttribute("posy", &y);
	pcam->QueryFloatAttribute("posz", &z);

	float rx, ry, rz;
	pcam->QueryFloatAttribute("rx", &rx);
	pcam->QueryFloatAttribute("ry", &ry);
	pcam->QueryFloatAttribute("rz", &rz);

	float upx, upy, upz;
	pcam->QueryFloatAttribute("upx", &upx);
	pcam->QueryFloatAttribute("upy", &upy);
	pcam->QueryFloatAttribute("upz", &upz);

	float fov;
	pcam->QueryFloatAttribute("fov", &fov);
	
	Camera cam = Camera(Vector3D(x, y, z), Vector3D(rx, ry, rz), Vector3D(upx, upy, upz), fov);

	std::vector<Mesh> objects;
	XMLElement * pobjs = pRoot->FirstChildElement("objs");
	for (XMLElement* pobj = pobjs->FirstChildElement("item"); pobj != NULL; pobj = pobj->NextSiblingElement("item")) {
		string path = std::string(pobj->Attribute("path"));
		Mesh m = Mesh(path);
		float trsx, trsy, trsz;
		pobj->QueryFloatAttribute("trsx", &trsx);
		pobj->QueryFloatAttribute("trsy", &trsy);
		pobj->QueryFloatAttribute("trsz", &trsz);
		Vector3D trs = Vector3D(trsx, trsy, trsz);
		m.translate(trs);
		objects.push_back(m);
	}

	std::vector<Light*> lights;
	XMLElement * plights = pRoot->FirstChildElement("lights");
	for (XMLElement* plight = plights->FirstChildElement("item"); plight != NULL; plight = plight->NextSiblingElement("item")) {
		string type = std::string(plight->Attribute("type"));
		float intensity;
		plight->QueryFloatAttribute("int", &intensity);
		float x, y, z;
		plight->QueryFloatAttribute("x", &x);
		plight->QueryFloatAttribute("y", &y);
		plight->QueryFloatAttribute("z", &z);
		if (type == "point") {
			PointLight* pl = new PointLight(Vector3D(x,y,z), intensity);
			lights.push_back(pl);
		}
		else if (type == "dir") {
			DirectionalLight* dir = new DirectionalLight(Vector3D(x, y, z), intensity);
			lights.push_back(dir);
		}
	}
	return new Scene(objects, lights, cam);
}

Scene::Scene(vector<Mesh> objects, vector<Light*> lights, Camera cam) {
	this->causticMap = NULL;
	this->photonMap = NULL;
	this->cam = cam;
	this->lights = lights;
	this->objects = objects;
}