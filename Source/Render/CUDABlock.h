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


	unsigned int m_VBO_Vertices;
	unsigned int m_VBO_Normals;
	unsigned int m_VBO_Indices;
	unsigned int m_VBO_UVs;
	cudaGraphicsResource *cuda_VBO_Vertices;
	cudaGraphicsResource *cuda_VBO_Normals;
	cudaGraphicsResource *cuda_VBO_Indices;
	cudaGraphicsResource *cuda_VBO_UVs;
	float3* cuda_Vertices;
	float3* cuda_Normals;
	unsigned int* cuda_Indices;
	float2* cuda_UVs;

	unsigned int m_FaceCount;

	unsigned int m_Rank;

	float3 mPos;

	static uint *cuda_voxelVerts ;
	static uint *cuda_voxelVertsScan ;
	static uint *cuda_voxelOccupied ;
	static uint *cuda_voxelOccupiedScan ;
	static uint *cuda_compVoxelArray ;
};