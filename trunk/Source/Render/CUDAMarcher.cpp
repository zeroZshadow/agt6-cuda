

//-- OpenGL implementation headers
#include "windows.h"
#include "glew.h"
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "glut.h"
#include "freeglut_ext.h"
//-- GL End
#include <cutil_inline.h>
#include <cuda_gl_interop.h>


#include "CUDAMarcher.h"
#include "CUDABlock.h"
#include <malloc.h>
#include "../Misc/Perlin.h"
#include "Defines.h"


extern "C" void load_tables_as_textures(unsigned int** aEdgeTable, unsigned int** aTriTable, unsigned int** aNrVertsTable);
extern "C" void host_PerlinInitialize(unsigned int nSeed);
extern "C" void host_InitPerlinData(int rank, int size);


CUDAMarcher::CUDAMarcher()
{
	m_BlockCount = 0;
	m_Blocks = 0;
	mPerlin = new CPerlin();
	mPerlin->Initialize(1);

	PrepareTerrain();
}

CUDAMarcher::~CUDAMarcher()
{
	delete[] m_Blocks;
	m_BlockCount = 0;
}

void CUDAMarcher::PrepareTerrain()
{
	mCudaEdgeTable = 0;
	mCudaTriTable = 0;
	mCudaVertTable = 0;
	mCudaPerlinDst1 = 0;
	mCudaPerlinDst2 = 0;
	mCudaPerlinDst3 = 0;

	//-- Load tables
	load_tables_as_textures( &mCudaEdgeTable, &mCudaTriTable, &mCudaVertTable );

	//-- Create and load perlin data
	host_PerlinInitialize(0);
	host_InitPerlinData(PERLIN_DATA_RANK, PERLIN_DATA_SIZE);

	//Init blocks
	Init(3, 2, 3);
}

void CUDAMarcher::GenerateTerrain(GenerateInfo gInfo)
{
	int now = timeGetTime();

	mGenInfo = gInfo;
	Cubemarch();

	int result = timeGetTime() - now;
	printf("> Created terrain, took %i ms\n", result);
}

void CUDAMarcher::ClearTerrain()
{
	//delete [] m_Blocks;
}

void CUDAMarcher::Init(int gridX, int gridY, int gridZ)
{
	m_BlockCount = gridZ*gridY*gridX;
	m_Blocks = new CUDABlock[m_BlockCount];


	for (int x = 0; x < gridX; x++)
	{
		for (int y = 0; y < gridY; y++)
		{
			for (int z = 0; z < gridZ; z++)
			{
				m_Blocks[(x * gridY * gridZ) + y * gridZ + z].Init(x,y,z);
			}
		}
	}
}

void CUDAMarcher::GenerateTileablePerlin(float* data, float scale, int rank)
{
	//mPerlin->Noise3((float)(x) / scale,(float)(y) / scale,(float)(z) / scale);
	int indecies[8][3];
	for (int x = 0; x < rank; x++)
	{
		for (int y = 0; y < rank; y++)
		{
			for (int z = 0; z < rank; z++)
			{
				indecies[0][0] = x; indecies[0][1] = y; indecies[0][2] = z;
				indecies[1][0] = x; indecies[1][1] = y; indecies[1][2] = z;
				indecies[2][0] = x;	indecies[2][1] = y;	indecies[2][2] = z;
				indecies[3][0] = x;	indecies[3][1] = y;	indecies[3][2] = z;
				indecies[4][0] = x+rank; indecies[4][1] = y+rank; indecies[4][2] = z+rank;
				indecies[5][0] = x; indecies[5][1] = y; indecies[5][2] = z;
				indecies[6][0] = x; indecies[6][1] = y; indecies[6][2] = z;
				indecies[7][0] = x; indecies[7][1] = y; indecies[7][2] = z;

				
				int index = (x * (int)rank * (int)rank) + (y * (int)rank) + z;
				//data1[ index ] = 
			}
		}
	}
}


void CUDAMarcher::Cubemarch()
{
	for(unsigned int i=0; i<m_BlockCount; ++i)
	{
		m_Blocks[i].Build(&mGenInfo);
	}
}

void CUDAMarcher::Render()
{
	for(unsigned int i=0; i<m_BlockCount; ++i)
	{
		m_Blocks[i].Render();
	}
}