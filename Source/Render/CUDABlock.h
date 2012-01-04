#pragma once
#include "../Misc/Vector3.h"

struct cudaGraphicsResource;
class CUDABlock
{
public:
	CUDABlock();
	~CUDABlock();

	void Init();
	void Build();
	void Render();

	unsigned int m_VBO_Vertices;
	unsigned int m_VBO_Normals;
	unsigned int m_VBO_Indices;
	cudaGraphicsResource *cuda_VBO_Vertices;
	cudaGraphicsResource *cuda_VBO_Normals;
	cudaGraphicsResource *cuda_VBO_Indices;
	Vector3* cuda_Vertices;
	Vector3* cuda_Normals;
	unsigned int* cuda_Indices;

	unsigned int m_FaceCount;

	unsigned int m_Rank;

	int m_X;
	int m_Y;
	int m_Z;
};