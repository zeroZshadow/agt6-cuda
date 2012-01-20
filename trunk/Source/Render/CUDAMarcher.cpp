#include "CUDAMarcher.h"
#include "CUDABlock.h"
#include <malloc.h>
#include "../Misc/Perlin.h"
#include "Defines.h"

extern "C" void load_tables_as_textures(unsigned int** aEdgeTable, unsigned int** aTriTable, unsigned int** aNrVertsTable);
extern "C" void load_perlin_data(float** aDstCuda1, float* aData1, float** aDstCuda2, float* aData2, float** aDstCuda3, float* aData3, unsigned int dataSize);


CUDAMarcher::CUDAMarcher()
{
	m_BlockCount = 0;
	m_Blocks = 0;
	mPerlin = new CPerlin();
	mPerlin->Initialize(1);
}

CUDAMarcher::~CUDAMarcher()
{
	delete[] m_Blocks;
	m_BlockCount = 0;
}

void CUDAMarcher::Init(int gridX, int gridY, int gridZ)
{
	m_BlockCount = gridZ*gridY*gridX;
	m_Blocks = new CUDABlock[m_BlockCount];

	mCudaEdgeTable = 0;
	mCudaTriTable = 0;
	mCudaVertTable = 0;
	mCudaPerlinDst1 = 0;
	mCudaPerlinDst2 = 0;
	mCudaPerlinDst3 = 0;

	//-- Load tables
	load_tables_as_textures( &mCudaEdgeTable, &mCudaTriTable, &mCudaVertTable );

	//-- Create and load perlin data
	float*	perlin1, *perlin2, *perlin3;
	int		rank		= PERLIN_DATA_RANK;
	int		perlinBlockSize		= rank * rank * rank;

	perlin1 = (float*)malloc( sizeof( float ) * perlinBlockSize );
	perlin2 = (float*)malloc( sizeof( float ) * perlinBlockSize );
	perlin3 = (float*)malloc( sizeof( float ) * perlinBlockSize );

	BuildPerlinData(perlin1,perlin2, perlin3, rank);
	load_perlin_data( &mCudaPerlinDst1, perlin1, &mCudaPerlinDst2, perlin2, &mCudaPerlinDst3, perlin3, perlinBlockSize );

	free( perlin1 );
	free( perlin2 );
	free( perlin3 );

	//for(unsigned int i=0; i<m_BlockCount; ++i)
	//{
	//	m_Blocks[i].Init(i, 0 ,0);
	//}
	for (int x = 0; x < gridX; x++)
	{
		for (int y = 0; y < gridY; y++)
		{
			for (int z = 0; z < gridZ; z++)
			{
				m_Blocks[(x * gridY * gridZ) + y * gridZ + z].Init(x,-y,z);
			}
		}
	}
}

void CUDAMarcher::BuildPerlinData(float* data1, float* data2, float* data3, unsigned int rankSize)
{
	int		index			= 0;

	//-- Quick and dirty pie
#define PI				3.14159265358979323846264338327950288419716939937510582097494459072381640628620899862803482534211706798f
	float	piDev				= PI * 5;
	float	piDev2				= PI * 1;
	float	piDev3				= PI * 0.1;
#undef PI

	for (int x = 0; x < rankSize; x++)
	{
		for (int y = 0; y < rankSize; y++)
		{
			for (int z = 0; z < rankSize; z++)
			{
				index = (x * (int)rankSize * (int)rankSize) + (y * (int)rankSize) + z;
				data1[ index ] = mPerlin->Noise3((float)(x) / piDev,(float)(y) / piDev,(float)(z) / piDev);
				data2[ index ] = mPerlin->Noise3((float)(x + rankSize*2) / piDev2,(float)(y) / piDev2,(float)(z) / piDev2);
				data3[ index ] = mPerlin->Noise3((float)(x + rankSize*4) / piDev3,(float)(y) / piDev3,(float)(z) / piDev3);
			}
		}
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