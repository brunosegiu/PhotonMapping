#include "Mesh.h"
#include <iostream>
#include<fstream>
#include<string>
#include<iterator>
#include <map>


//#include "cuda_runtime.h"

using namespace std;

map<string, Material*> loadMtl(string path) {
	string mtlExt = path + ".mtl";
	string line;
	ifstream mtlFile(mtlExt.c_str());
	map<string, Material*> mats;
	if (mtlFile.is_open()) {

		string currentMtl = "";
		string prefixMTL = "newmtl";
		string prefixKa = "Ka";
		string prefixKd = "Kd";
		string prefixKs = "Ks";
		string prefixNs = "Ns";
		string prefixMapKd = "map_Kd";
		string prefixRl = "Rl";
		string prefixTs = "Ts";
		string prefixTp = "Tp";

		float aux1, aux2, aux3;
		float ambient1, ambient2, ambient3 = 0;
		float diffuse1, diffuse2, diffuse3 = 0;
		float specular1, specular2, specular3 = 0;
		float shininess = 0;
		float refl, trans;
		refl = trans = 0.0f;
		float transp = 0.0f;
		string texturePath = "";

		while (!mtlFile.eof()) {
			getline(mtlFile, line);
			if (!strncmp(line.c_str(), prefixMTL.c_str(), strlen(prefixMTL.c_str()))) {
				if (currentMtl != "") {
					mats[currentMtl] = new Material(RGBA(ambient1, ambient2, ambient3), RGBA(diffuse1, diffuse2, diffuse3), RGBA(specular1, specular2, specular3), shininess, texturePath, refl, trans, transp);
				}
				currentMtl = line.substr(7, line.length());
			}
			else if (!strncmp(line.c_str(), prefixKa.c_str(), strlen(prefixKa.c_str()))) {
				string data = line.substr(strlen(prefixKa.c_str()) - 1, strlen(line.c_str()));
				data[0] = data[1] = ' ';
				sscanf_s(data.c_str(), "%f %f %f", &aux1, &aux2, &aux3);
				ambient1 = aux1;
				ambient2 = aux2;
				ambient3 = aux3;
			}
			else if (!strncmp(line.c_str(), prefixKd.c_str(), strlen(prefixKd.c_str()))) {
				string data = line.substr(strlen(prefixKd.c_str()) - 1, strlen(line.c_str())).c_str();
				data[0] = data[1] = ' ';
				sscanf_s(data.c_str(), "%f %f %f", &aux1, &aux2, &aux3);
				diffuse1 = aux1;
				diffuse2 = aux2;
				diffuse3 = aux3;
			}
			else if (!strncmp(line.c_str(), prefixNs.c_str(), strlen(prefixNs.c_str()))) {
				string data = line.substr(strlen(prefixNs.c_str()) - 1, strlen(line.c_str())).c_str();
				data[0] = data[1] = ' ';
				sscanf_s(data.c_str(), "%f ", &aux1);
				shininess = aux1;
			}
			else if (!strncmp(line.c_str(), prefixKs.c_str(), strlen(prefixKs.c_str()))) {
				string data = line.substr(strlen(prefixKs.c_str()) - 1, strlen(line.c_str())).c_str();
				sscanf_s(data.c_str(), "%f %f %f", &aux1, &aux2, &aux3);
				specular1 = aux1;
				specular2 = aux2;
				specular3 = aux3;
			}
			else if (!strncmp(line.c_str(), prefixMapKd.c_str(), strlen(prefixMapKd.c_str()))) {
				texturePath = line.substr(strlen(prefixMapKd.c_str()) + 1, strlen(line.c_str())).c_str();
			}else if (!strncmp(line.c_str(), prefixRl.c_str(), strlen(prefixRl.c_str()))) {
				string data = line.substr(strlen(prefixRl.c_str()) - 1, strlen(line.c_str())).c_str();
				data[0] = data[1] = ' ';
				sscanf_s(data.c_str(), "%f ", &aux1);
				refl = aux1;
			}else if (!strncmp(line.c_str(), prefixTs.c_str(), strlen(prefixTs.c_str()))) {
				string data = line.substr(strlen(prefixTs.c_str()) - 1, strlen(line.c_str())).c_str();
				data[0] = data[1] = ' ';
				sscanf_s(data.c_str(), "%f ", &aux1);
				trans = aux1;
			}else if (!strncmp(line.c_str(), prefixTp.c_str(), strlen(prefixTp.c_str()))) {
				string data = line.substr(strlen(prefixTp.c_str()) - 1, strlen(line.c_str())).c_str();
				data[0] = data[1] = ' ';
				sscanf_s(data.c_str(), "%f ", &aux1);
				transp = aux1;
			}
		}

		if (currentMtl != "" && mats.count(currentMtl) == 0) {
			mats[currentMtl] = new Material(RGBA(ambient1, ambient2, ambient3), RGBA(diffuse1, diffuse2, diffuse3), RGBA(specular1, specular2, specular3), shininess, texturePath, refl, trans, transp);
		}

		mtlFile.close();
		return mats;
	}
	else {
		printf("NO SE PUDO ABRIR EL ARCHIVO");
	}
}

float* resize(float * arr, unsigned int elem) {
	float* new_arr = NULL;
	if (elem > 0) {
		new_arr = (float*)malloc(sizeof(float*) * elem);
		for (unsigned int i = 0; i < elem; i++) {
			new_arr[i] = arr[i];
		}
	}
	if (arr) {
		delete[] arr;
	}
	return new_arr;
}

Mesh::Mesh(string path){
	this->path = path;
	map<string, float*> normals;
	map<string, float*> faces;
	map<string, float*> textures;
	map<string, int> faceVertices;
	Vector3D maxCoords = Vector3D(-INFINITY, -INFINITY, -INFINITY);
	Vector3D minCoords = Vector3D(INFINITY, INFINITY, INFINITY);
	string fullpath = path + ".obj";
	map<string, Material*> mats = loadMtl(path);
	string line;
	ifstream objFile (fullpath.c_str());
	if (objFile.is_open()) {
		objFile.seekg(0, ios::end);
		long fileSize = long(objFile.tellg());
		objFile.seekg(0, ios::beg);

		string currentMtl = "";

		float* vertexBuffer = new float[fileSize];
		float* textureBuffer = new float[fileSize];
		float* normalBuffer = new float[fileSize];

		int triangleIndex = 0;
		int normalIndex = 0;
		int textureCoordIndex = 0;

		int connectedVert = 0;
		int textureCoords = 0;
		int normalsCount = 0;

		string prefix = "usemtl";

		while (!objFile.eof()) {
			getline(objFile, line);
			if (line.c_str()[0] == 'v' && line.c_str()[1] == ' ') {
				line[0] = ' ';

				sscanf_s(line.c_str(), "%f %f %f ",
					&vertexBuffer[connectedVert],
					&vertexBuffer[connectedVert + 1],
					&vertexBuffer[connectedVert + 2]);

				//Buscar min - max
				if (vertexBuffer[connectedVert] < minCoords.x)
					minCoords.x = vertexBuffer[connectedVert];
				if (vertexBuffer[connectedVert + 1] < minCoords.y)
					minCoords.y = vertexBuffer[connectedVert + 1];
				if (vertexBuffer[connectedVert + 2] < minCoords.z)
					minCoords.z = vertexBuffer[connectedVert + 2];

				if (vertexBuffer[connectedVert] > maxCoords.x)
					maxCoords.x = vertexBuffer[connectedVert];
				if (vertexBuffer[connectedVert + 1] > maxCoords.y)
					maxCoords.y = vertexBuffer[connectedVert + 1];
				if (vertexBuffer[connectedVert + 2] > maxCoords.z)
					maxCoords.z = vertexBuffer[connectedVert + 2];

				connectedVert += 3;
			}
			else if (line.c_str()[0] == 'v' && line.c_str()[1] == 't') {
				line[0] = ' ';
				line[1] = ' ';

				sscanf_s(line.c_str(), "%f %f ",
					&textureBuffer[textureCoords],
					&textureBuffer[textureCoords + 1]);
				textureCoords += 2;
			}
			else if (line.c_str()[0] == 'v' && line.c_str()[1] == 'n') {
				line[0] = ' ';
				line[1] = ' ';

				sscanf_s(line.c_str(), "%f %f %f ",
					&normalBuffer[normalsCount],
					&normalBuffer[normalsCount + 1],
					&normalBuffer[normalsCount + 2]);
				normalsCount += 3;
			}
			else if (line.c_str()[0] == 'f') {
				line[0] = ' ';
				int vertexNumber[3] = { 0,0,0 };
				int normalNumber[3] = { 0,0,0 };
				int textureNumber[3] = { 0,0,0 };
				if (textureCoords > 0) {
					sscanf_s(line.c_str(), "%i/%i/%i %i/%i/%i %i/%i/%i",
						&vertexNumber[0], &textureNumber[0], &normalNumber[0],
						&vertexNumber[1], &textureNumber[1], &normalNumber[1],
						&vertexNumber[2], &textureNumber[2], &normalNumber[2]);
				}
				else {
					sscanf_s(line.c_str(), "%i//%i %i//%i %i//%i",
						&vertexNumber[0], &normalNumber[0],
						&vertexNumber[1], &normalNumber[1],
						&vertexNumber[2], &normalNumber[2]);
				}

				for (int j = 0; j < 3; j++) {
					vertexNumber[j] -= 1;
					normalNumber[j] -= 1;
					textureNumber[j] -= 1;
				}

				for (int i = 0; i < 3; i++) {
					faces[currentMtl][triangleIndex] = vertexBuffer[3 * vertexNumber[i]];
					faces[currentMtl][triangleIndex + 1] = vertexBuffer[3 * vertexNumber[i] + 1];
					faces[currentMtl][triangleIndex + 2] = vertexBuffer[3 * vertexNumber[i] + 2];
					triangleIndex += 3;

					normals[currentMtl][normalIndex] = normalBuffer[3 * normalNumber[i]];
					normals[currentMtl][normalIndex + 1] = normalBuffer[3 * normalNumber[i] + 1];
					normals[currentMtl][normalIndex + 2] = normalBuffer[3 * normalNumber[i] + 2];
					normalIndex += 3;

					if (textureCoords > 0) {
						textures[currentMtl][textureCoordIndex] = textureBuffer[2 * textureNumber[i]];
						textures[currentMtl][textureCoordIndex + 1] = textureBuffer[2 * textureNumber[i] + 1];
						textureCoordIndex += 2;
					}
				}
				faceVertices[currentMtl] += 9;
			}
			else if (!strncmp(line.c_str(), prefix.c_str(), strlen(prefix.c_str()))) {
				if (currentMtl != "") {
					faces[currentMtl] = resize(faces[currentMtl], faceVertices[currentMtl]);;
					normals[currentMtl] = resize(normals[currentMtl], faceVertices[currentMtl]);
					textures[currentMtl] = resize(textures[currentMtl], 2 * faceVertices[currentMtl] / 3);
				}
				currentMtl = line.substr(7, line.length());
				faces[currentMtl] = new float[fileSize];
				normals[currentMtl] = new float[fileSize];
				textures[currentMtl] = new float[fileSize];
				faceVertices[currentMtl] = 0;
				triangleIndex = 0;
				normalIndex = 0;
				textureCoordIndex = 0;
			}

		}
		objFile.close();
		delete[] vertexBuffer;
		delete[] textureBuffer;
		delete[] normalBuffer;
		for (map<string, float*>::iterator it = faces.begin(); it != faces.end(); it++){
			string mtl = it->first;
			MeshComponent* mcp = new MeshComponent(faces[mtl], normals[mtl], textures[mtl] , mats[mtl], faceVertices[mtl]);
			parts.push_back(mcp);
		}
		this->boundingbox = AABB(minCoords, maxCoords);
		this->acumTranslate = Vector3D();
	}
	else {
		printf("NO SE PUDO ABRIR EL ARCHIVO");
	}
}

Vector3D* Mesh::intersect(Vector3D &rayorig, Vector3D & raydir, MeshComponent* &curr, unsigned int &index, float &u, float &v) {
	float currDist, tempDist;
	currDist = tempDist = 999999999999.0f;
	Vector3D* inter, *tempInter = NULL;
	float ut, vt;
	inter = NULL;
	unsigned int tempIndex;
	for (unsigned int i = 0; i < this->parts.size(); i++) {
		tempInter = this->parts[i]->intersect(rayorig, raydir, tempIndex, ut, vt);
		if (tempInter) {
			tempDist = tempInter->GetDistance(rayorig);
			if (tempDist < currDist) {
				currDist = tempDist;
				curr = this->parts[i];
				if (inter) delete inter;
				inter = tempInter;
				index = tempIndex;
				u = ut;
				v = vt;
			}
		}
	}
	return inter;
}

Mesh::~Mesh() {

}

void Mesh::translate(Vector3D d) {
	this->acumTranslate += d;
	this->boundingbox.max += d;
	this->boundingbox.min += d;
	for (unsigned int i = 0; i < parts.size(); i++) {
		for (unsigned int j = 0; j < parts[i]->vertexCount; j += 9) {
			parts[i]->vertexBuffer[j] += d.x;
			parts[i]->vertexBuffer[j+1] += d.y;
			parts[i]->vertexBuffer[j+2] += d.z;
		}
	}
}