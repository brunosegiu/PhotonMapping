#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "CUDA_Intersect.cuh"
#include <stdio.h>

#define INFINITE 1e30

__device__ vertex operator +(vertex &v0, vertex &v1) {
	vertex ret = { v0.x + v1.x, v0.y + v1.y, v0.z + v1.z };
	return ret;
}

__device__ vertex operator - (vertex &v0, vertex &v1) {
	vertex ret = { v0.x - v1.x, v0.y - v1.y, v0.z - v1.z };
	return ret;
}

__device__ float dot(vertex &v0, vertex &v1) {
	return v0.x*v1.x + v0.y * v1.y + v0.z * v1.z;
}

__device__ vertex operator *(vertex &v0, float &f) {
	vertex ret = { v0.x * f, v0.y * f, v0.z * f };
	return ret;
}

__device__ vertex cross(vertex &v0, vertex &v1) {
	vertex ret = { v0.y * v1.z - v0.z * v1.y , v0.z * v1.x - v0.x * v1.z, v0.x  * v1.y - v0.y * v1.x };
	return ret;
}

__global__ void moller_trumbore(vertex raydir, vertex rayorig, float* vertexBuffer, float* hit, float* distance, float* uv, unsigned int triangles){
	unsigned int triangleNumber = blockIdx.x * blockDim.x + threadIdx.x;
	if (triangleNumber < triangles) {
		unsigned int toff = 9 * triangleNumber;
		distance[triangleNumber] = -1.0f;
		vertex v0 = { vertexBuffer[toff] , vertexBuffer[toff + 1], vertexBuffer[toff + 2] };
		vertex e1 = { vertexBuffer[toff + 3] , vertexBuffer[toff + 4], vertexBuffer[toff + 5] };
		vertex e2 = { vertexBuffer[toff + 6] , vertexBuffer[toff + 7], vertexBuffer[toff + 8] };
		vertex pvec = cross(raydir, e2);
		float inv_det = 1.0f / dot(e1, pvec);
		vertex tvec = rayorig - v0;
		float ut = dot(tvec, pvec) * inv_det;
		if (ut >= 0 && ut <= 1) {
			vertex qvec = cross(tvec, e1);
			float vt = dot(raydir, qvec) * inv_det;
			if (vt >= 0 && ut + vt <= 1) {
				float dist = dot(e2, qvec) * inv_det;
				vertex inter = raydir * dist + rayorig;
				unsigned int hitoff = 3 * triangleNumber;
				hit[hitoff] = inter.x;
				hit[hitoff + 1] = inter.y;
				hit[hitoff + 2] = inter.z;
				distance[triangleNumber] = dist;
				uv[2 * triangleNumber] = ut;
				uv[2 * triangleNumber + 1] = vt;
			}
		}
	}
}

Vector3D* CUDA_Intersect(Vector3D &rayorig, Vector3D &raydir, unsigned int &index, float &u, float &v, float* vertexBuffer, unsigned int vertexCount){

	float *hits_out_dev, *distance_out_dev, *uv_out_dev;

	size_t sizeh, sized, sizeuv;
	sizeh = (sizeof(float) * vertexCount) / 3;
	sized = (sizeof(float) * vertexCount) / 9;
	sizeuv = (2 * sizeof(float) * vertexCount) / 9;

	cudaError_t ce;
	do {
		ce = cudaMalloc(&hits_out_dev, sizeh);
	} while (ce != cudaSuccess);
	do {
		ce = cudaMalloc(&distance_out_dev, sized);
	} while (ce != cudaSuccess);
	do {
		ce = cudaMalloc(&uv_out_dev, sizeuv);
	} while (ce != cudaSuccess);

	dim3 tamGrid((vertexCount / (CUDABLOCKSIZE * 9)), 1);
	dim3 tamBlock(CUDABLOCKSIZE, 1);

	vertex rd, ro;
	rd = { raydir.x,raydir.y,raydir.z };
	ro = { rayorig.x,rayorig.y,rayorig.z };

	moller_trumbore <<< tamGrid, tamBlock >>> (rd, ro, vertexBuffer, hits_out_dev, distance_out_dev, uv_out_dev, vertexCount/9);

	cudaDeviceSynchronize();

	float * hits, *distance, *uv;
	hits =  (float*) malloc(sizeh);
	distance = (float*)malloc(sized);
	uv = (float*)malloc(sizeuv);

	cudaMemcpy(hits, hits_out_dev,sizeh, cudaMemcpyDeviceToHost);
	cudaMemcpy(distance, distance_out_dev,sized, cudaMemcpyDeviceToHost);
	cudaMemcpy(uv, uv_out_dev,sizeuv, cudaMemcpyDeviceToHost);

	float dist = INFINITE;
	Vector3D* hit = new Vector3D();
	index = 0;
	for (unsigned int i = 0; i < vertexCount / 9; i++) {
		if (distance[i] > 1e-7 && distance[i] < dist) {
			hit->x = hits[3*i];
			hit->y = hits[3*i + 1];
			hit->z = hits[3 * i + 2];
			u = uv[2 * i];
			v = uv[2 * i + 1];
			dist = distance[i];
			index = i;
		}
	}

	if (dist == -1.0f) {
		delete hit;
		hit = NULL;
	}

	delete[] hits;
	delete[] distance;
	delete[] uv;

	cudaFree(hits_out_dev);
	cudaFree(distance_out_dev);
	cudaFree(uv_out_dev);

	return hit;
}

