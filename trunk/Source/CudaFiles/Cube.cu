#include <cutil_inline.h>
#include <cuda_runtime_api.h>
#include <thrust/device_vector.h>
#include <thrust/scan.h>
#include "cutil_math.h"
#include "../Render/CUDATables.h"
#include "../Render/Defines.h"

//-- Table textures
texture<unsigned int, 1, cudaReadModeElementType> tEdgeTex;
texture<unsigned int, 1, cudaReadModeElementType> tNRVertsTex;
texture<unsigned int, 1, cudaReadModeElementType> tTriTex;

//-- Sampling data
texture<unsigned char, 1, cudaReadModeNormalizedFloat> tData1;
texture<unsigned char, 1, cudaReadModeNormalizedFloat> tData2; //-- octave 2
texture<unsigned char, 1, cudaReadModeNormalizedFloat> tData3; //-- octave 3


//-- load tables into 
extern "C"
void load_tables_as_textures(unsigned int** aEdgeTable, unsigned int** aTriTable, unsigned int** aNrVertsTable)
{
	// set up format
    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindUnsigned);

	// allocate tables as textures
    cutilSafeCall(cudaMalloc((void**) aEdgeTable, 256*sizeof(unsigned int)));
    cutilSafeCall(cudaMemcpy((void*)* aEdgeTable, (void *)edgeTable, 256*sizeof(unsigned int), cudaMemcpyHostToDevice) );
    cutilSafeCall(cudaBindTexture(0, tEdgeTex, *aEdgeTable, channelDesc) );

    cutilSafeCall(cudaMalloc((void**) aNrVertsTable, 256*sizeof(unsigned int)));
    cutilSafeCall(cudaMemcpy((void*)* aNrVertsTable, (void *)numVertsTable, 256*sizeof(unsigned int), cudaMemcpyHostToDevice) );
    cutilSafeCall(cudaBindTexture(0, tNRVertsTex, *aNrVertsTable, channelDesc) );

    cutilSafeCall(cudaMalloc((void**) aTriTable, 256*16*sizeof(unsigned int)));
    cutilSafeCall(cudaMemcpy((void*)*aTriTable, (void *)triTable, 256*16*sizeof(unsigned int), cudaMemcpyHostToDevice) );
    cutilSafeCall(cudaBindTexture(0, tTriTex, *aTriTable, channelDesc) );
}

extern "C"
void load_perlin_data(float** aDstCuda1, float* aData1, float** aDstCuda2, float* aData2, float** aDstCuda3, float* aData3, unsigned int dataSize)
{
	cudaChannelFormatDesc cD = cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat);

	// allocate data as textures
    cutilSafeCall(cudaMalloc((void**) aDstCuda1, dataSize*sizeof(float)));
    cutilSafeCall(cudaMemcpy((void*)* aDstCuda1, (void *)aData1, dataSize*sizeof(float), cudaMemcpyHostToDevice) );
	cutilSafeCall(cudaBindTexture(0, tData1, *aDstCuda1, cD) );

    cutilSafeCall(cudaMalloc((void**) aDstCuda2, dataSize*sizeof(float)));
    cutilSafeCall(cudaMemcpy((void*)* aDstCuda2, (void *)aData2, dataSize*sizeof(float), cudaMemcpyHostToDevice) );
    cutilSafeCall(cudaBindTexture(0, tData2, *aDstCuda2, cD) );

    cutilSafeCall(cudaMalloc((void**) aDstCuda3, dataSize*sizeof(float)));
    cutilSafeCall(cudaMemcpy((void*)* aDstCuda3, (void *)aData3, dataSize*sizeof(float), cudaMemcpyHostToDevice) );
    cutilSafeCall(cudaBindTexture(0, tData3, *aDstCuda2, cD) );
}

__device__ float SampleData1( uint3 pos )
{
    unsigned int i = (pos.z*PERLIN_DATA_RANK*PERLIN_DATA_RANK) + (pos.y*PERLIN_DATA_RANK) + pos.x;
    return tex1Dfetch(tData1, i);
}

__device__ float SampleData2( uint3 pos )
{
	unsigned int i = (pos.z*PERLIN_DATA_RANK*PERLIN_DATA_RANK) + (pos.y*PERLIN_DATA_RANK) + pos.x;
    return tex1Dfetch(tData2, i);
}

__device__ float SampleData3( uint3 pos )
{
    unsigned int i = (pos.z*PERLIN_DATA_RANK*PERLIN_DATA_RANK) + (pos.y*PERLIN_DATA_RANK) + pos.x;
    return tex1Dfetch(tData3, i);
}


__global__ void cuda_CreateCube(float3* aVertList, float3* aNormList, unsigned int* aIndexList)
{
	int column = ( blockDim.x * blockIdx.x) + threadIdx.x;
	int row = ( blockDim.y * blockIdx.y) + threadIdx.y;
	int depth = ( blockDim.z * blockIdx.z) + threadIdx.z;

	int vertex = (column + (row * blockDim.x * gridDim.x)) + (depth * blockDim.x * gridDim.x * blockDim.y * gridDim.y);
	vertex *= 15;
	int triangle = vertex;

	//float xdim = (float)column * 0.05f;
	//float ydim = (float)row * 0.05f;
	//float zdim = (float)depth * 0.05f;

	{
	float points[8];
	unsigned int bitmap = 0;

		points[0] = SampleData1( make_uint3( row, column, depth ) );
		points[1] = SampleData1( make_uint3( row, column+1, depth ) );
		points[2] = SampleData1( make_uint3( row+1, column+1, depth ) );
		points[3] = SampleData1( make_uint3( row+1, column, depth ) );
		points[4] = SampleData1( make_uint3( row, column, depth+1 ) );
		points[5] = SampleData1( make_uint3( row, column+1, depth+1 ) );
		points[6] = SampleData1( make_uint3( row+1, column+1, depth+1 ) );
		points[7] = SampleData1( make_uint3( row+1, column, depth+1 ) );

		//--Create lookup bitmap to find the edge table
		for (int i = 0; i < 8; i++)
		{
			if (points[i] > 0)
			{
				bitmap ^= 1<<i;						
			}
		}

		//-- Creating triangles
		unsigned int edge = tex1Dfetch(tEdgeTex, bitmap);
		unsigned int vertNr = tex1Dfetch(tNRVertsTex ,bitmap);
		if (vertNr >= 255 || vertNr <= 0)
		{

		}
		else
		{
			float3 vertsPos[12]; 
			//-- front
			vertsPos[0]  = make_float3(0,		0.5,	0);
			vertsPos[1]  = make_float3(0.5f,	1,		0);
			vertsPos[2]  = make_float3(1,		0.5f,	0);
			vertsPos[3]  = make_float3(0.5f,	0,		0);

			//-- back
			vertsPos[4]  = make_float3(0,		0.5f,	1);
			vertsPos[5]  = make_float3(0.5f,	1,		1);
			vertsPos[6]  = make_float3(1,		0.5f,	1);
			vertsPos[7]  = make_float3(0.5f,	0,		1);

			//-- mid
			vertsPos[8]  = make_float3(0,		0,		0.5f);
			vertsPos[9]  = make_float3(0,		1,		0.5f);
			vertsPos[10]  = make_float3(1,		1,		0.5f);
			vertsPos[11]  = make_float3(1,		0,		0.5f);
		

			for (int i = 0; i < vertNr; i++)
			{
				aVertList[vertex] = vertsPos[tex1Dfetch(tTriTex, (bitmap * 16) + i)];
				aNormList[vertex] = make_float3(0,0,0);
				aVertList[vertex] += make_float3( row, column, depth);
				aVertList[vertex] *= make_float3( 0.05, 0.05, 0.05);
				aIndexList[triangle] = vertex++;
				triangle++;
				

				//aVertList[vertex] = vertsPos[triTablePC[bitmap][i]];
				//aVertList[vertex] += make_float3( row, column, depth );
				//aTriList[triangle] = vertex++;
				//triangle++;
				//
				//aVertList[vertex] = vertsPos[triTablePC[bitmap][i]];
				//aVertList[vertex] += make_float3( row, column, depth );
				//aTriList[triangle] = vertex++;
				//triangle++;
			}

		}


	}	
}

__global__ void generateNormals(float3* aVertList, float3* aNormList, unsigned int* aIndexList)
{
	
}


extern "C"
void launch_CreateCube(dim3 grid, dim3 threads, float3* aVertList, float3* aNormList, unsigned int* aIndexList)
{
	//Here you normally run:
	cuda_CreateCube<<<grid, threads>>>(aVertList, aNormList, aIndexList);
	cutilCheckMsg("cuda_CreateCube failed");
	
}



//--the cuber
//__global__ void cuda_CreateCube(float3* aVertList, float3* aNormList, unsigned int* aIndexList)
//{
//	int column = ( blockDim.x * blockIdx.x) + threadIdx.x;
//	int row = ( blockDim.y * blockIdx.y) + threadIdx.y;
//	int depth = ( blockDim.z * blockIdx.z) + threadIdx.z;
//
//	int vertex = (column + (row * blockDim.x * gridDim.x)) + (depth * blockDim.x * gridDim.x * blockDim.y * gridDim.y);
//	vertex *= 3;
//	int triangle = vertex;
//
//	if( vertex < 500000)
//	{
//		float xdim = (float)column * 0.05;
//		float ydim = (float)row * 0.05;
//		float zdim = (float)depth * 0.05;
//		aVertList[vertex] = make_float3(0.01 + xdim,ydim, zdim);
//		aVertList[vertex+1] = make_float3(xdim, 0.01 + ydim, zdim);
//		aVertList[vertex+2] = make_float3(xdim, ydim, zdim);
//		
//		aNormList[vertex] = make_float3(0,0,1);
//		aNormList[vertex+1] = make_float3(0,0,1);
//		aNormList[vertex+2] = make_float3(0,0,1);
//		
//		aIndexList[triangle] = vertex;
//		aIndexList[triangle+1] = vertex+1;
//		aIndexList[triangle+2] = vertex+2;
//	}
//}