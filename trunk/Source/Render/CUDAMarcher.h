#pragma once;

#include "Defines.h"

class CPerlin;

class CUDABlock;
class CUDAMarcher
{
public:
	CUDAMarcher();
	~CUDAMarcher();

	void		PrepareTerrain();
	void		GenerateTerrain(GenerateInfo gInfo);
	void		ClearTerrain();
		
	void		Init(int gridX, int gridY, int gridZ);


	void		Cubemarch();

	void		Render();



protected:
	void GenerateTileablePerlin(float* data, float scale, int rank);

	GenerateInfo		mGenInfo;

	CPerlin*			mPerlin;

	unsigned int m_Rank;

	//Change later
	unsigned int m_BlockCount;
	CUDABlock* m_Blocks;

	unsigned int* mCudaEdgeTable;
	unsigned int* mCudaTriTable;
	unsigned int* mCudaVertTable;
	float* mCudaPerlinDst1;
	float* mCudaPerlinDst2;
	float* mCudaPerlinDst3;

	float* mCudaBlockPosition;
};