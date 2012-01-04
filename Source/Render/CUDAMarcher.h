#pragma once;


class CUDABlock;
class CUDAMarcher
{
public:
	CUDAMarcher();
	~CUDAMarcher();


	void		Init(unsigned int aRank, int aCount);
	void		Cubemarch();

	void		Render();

protected:
	unsigned int m_Rank;

	//Change later
	unsigned int m_BlockCount;
	CUDABlock* m_Blocks;


};