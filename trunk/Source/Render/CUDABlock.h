#pragma once
#include "vector_types.h"
class CPerlin;

struct cudaGraphicsResource;
class CUDABlock
{
public:
	CUDABlock();
	~CUDABlock();

	void Init(float x, float y, float z);
	void Build();
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

	int m_X;
	int m_Y;
	int m_Z;

};