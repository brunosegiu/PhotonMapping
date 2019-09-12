#include "MeshLight.h"


MeshLight::MeshLight(string path, float intensity, Scene* scene): Mesh(path){
	MeshComponent* component = NULL;
	float partsIntensity = intensity/(this->parts.size());
	for (unsigned int i=0; i<this->parts.size();i++){
		component = this->parts.at(i);

		for(unsigned int j=0; j<component->vertexCount; j+=9){
			Vector3D* v0 = new Vector3D(component->vertexBuffer[j], component->vertexBuffer[j +1], component->vertexBuffer[j +2]);
			Vector3D* e1 = new Vector3D(component->vertexBuffer[j+3], component->vertexBuffer[j +4], component->vertexBuffer[j +5]);
			Vector3D* e2 = new Vector3D(component->vertexBuffer[j +7], component->vertexBuffer[j +7], component->vertexBuffer[j +8]);
			
			//Baricentro de coordenadas 1/3(v0+e1+e2)
			
			Vector3D dir = e1->GetCross(e2).GetNormalized();
			Vector3D pos = v0->GetAdd(dir*0.01);
			//pos = pos + dir*0.001;
			//Pusheo cada luz aparte
			//scene->lights.push_back(new PointDirLight(pos,dir, partsIntensity));
			scene->lights.push_back(new PointLight(pos, partsIntensity));
			/*scene->lights.push_back(new PointDirLight(e1, dir, partsIntensity));
			scene->lights.push_back(new PointDirLight(e2, dir, partsIntensity));*/
			delete v0;
			delete e1;
			delete e2;
		}
	}
}

MeshLight::~MeshLight() {

}