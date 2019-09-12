#pragma once

#include <map>

using namespace std;

class CUDA_MemoryMapper {
public:
	map<int, float*> memmap;
	int count;
private:
	CUDA_MemoryMapper();
public:
	getArray();
};
