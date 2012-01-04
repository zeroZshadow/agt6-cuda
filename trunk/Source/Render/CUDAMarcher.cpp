#include "CUDAMarcher.h"
#include "CUDABlock.h"

CUDAMarcher::CUDAMarcher()
{
	m_BlockCount = 0;
	m_Blocks = 0;
}

CUDAMarcher::~CUDAMarcher()
{
	delete[] m_Blocks;
	m_BlockCount = 0;
}

void CUDAMarcher::Init(unsigned int aRank, int aCount)
{
	m_BlockCount = aCount;
	m_Blocks = new CUDABlock[m_BlockCount];

	for(unsigned int i=0; i<m_BlockCount; ++i)
	{
		m_Blocks[i].Init();
	}
}

void CUDAMarcher::Cubemarch()
{
	for(unsigned int i=0; i<m_BlockCount; ++i)
	{
		m_Blocks[i].Build();
	}
}

void CUDAMarcher::Render()
{
	for(unsigned int i=0; i<m_BlockCount; ++i)
	{
		m_Blocks[i].Render();
	}
}