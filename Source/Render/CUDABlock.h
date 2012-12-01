#pragma once
#include "vector_types.h"
#include <cutil_math.h>
class CPerlin;

struct GenerateInfo;
struct cudaGraphicsResource;
class CUDABlock
{
public:
	CUDABlock();
	~CUDABlock();

	void Init(float x, float y, float z);
	bool ResizeVBOs(int vertices, int indices);
	bool Build(GenerateInfo* agInfo);
	void Render();

	unsigned int m_VBO_Data;

	cudaGraphicsResource *cuda_VBO_Data;

	void* cuda_Data;

	unsigned int m_IndexCount;
	unsigned int m_VertexCount;
	size_t m_BytesUsed;

	unsigned int m_Rank;

	float3 mPos;

	static uint *cuda_voxelVerts ;
	static uint *cuda_voxelVertsScan ;
	static uint *cuda_voxelOccupied ;
	static uint *cuda_voxelOccupiedScan ;
	static uint *cuda_compVoxelArray ;
};