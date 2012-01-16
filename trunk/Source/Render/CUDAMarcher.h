#pragma once;

class CPerlin;

class CUDABlock;
class CUDAMarcher
{
public:
	CUDAMarcher();
	~CUDAMarcher();


	void		Init(unsigned int aRank, int aCount);
	void		Cubemarch();

	void		Render();

	void		BuildPerlinData(float* data1, float* data2, float* data3, unsigned int rankSize);
	


protected:
	CPerlin* mPerlin;

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
};