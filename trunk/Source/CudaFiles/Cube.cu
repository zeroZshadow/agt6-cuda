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

__device__ float SampleData1( int3 pos )
{
	//pos.x += 7;
	//pos.y += 7;
	//pos.z += 7;
	pos.x %= PERLIN_DATA_RANK;
	pos.y %= PERLIN_DATA_RANK;
	pos.z %= PERLIN_DATA_RANK;
    unsigned int i = (pos.z*PERLIN_DATA_RANK*PERLIN_DATA_RANK) + (pos.y*PERLIN_DATA_RANK) + pos.x;
    return tex1Dfetch(tData1, i);
}

__device__ float SampleData2( int3 pos )
{
	//pos.x += 11;
	//pos.y += 11;
	//pos.z += 11;	
	pos.x %= PERLIN_DATA_RANK;
	pos.y %= PERLIN_DATA_RANK;
	pos.z %= PERLIN_DATA_RANK;
	unsigned int i = (pos.z*PERLIN_DATA_RANK*PERLIN_DATA_RANK) + (pos.y*PERLIN_DATA_RANK) + pos.x;
    return tex1Dfetch(tData2, i);
}

__device__ float SampleData3( int3 pos )
{
	pos.x %= PERLIN_DATA_RANK;
	pos.y %= PERLIN_DATA_RANK;
	pos.z %= PERLIN_DATA_RANK;
    unsigned int i = (pos.z*PERLIN_DATA_RANK*PERLIN_DATA_RANK) + (pos.y*PERLIN_DATA_RANK) + pos.x;
    return tex1Dfetch(tData3, i);
}


__device__ float DensityWithFloor( int3 pos, float floorMultiplier)
{
	return SampleData1( pos ) + SampleData2( pos ) * 0.1f + SampleData3( pos ) * 0.05f - (pos.y * floorMultiplier);
}


__device__
float3 InterpVertexPos(float3 p0, float3 p1, float f0, float f1)
{
    float t = clamp(f0 / (f0 - f1), 0.0f , 1.0f);
	return lerp(p0, p1, t);
} 

__device__ 
float3 InterpVertexPos2(float3 p0, float3 p1, float f0, float f1)
{
    float t = clamp(f1 / (f1 - f0), 0.0f , 1.0f);
	return lerp(p0, p1, t);
}

__global__ void cuda_CreateCube(float3 pos, float3* aVertList, float3* aNormList, unsigned int* aIndexList)
{
	int column = ( blockDim.x * blockIdx.x) + threadIdx.x;
	int row = ( blockDim.y * blockIdx.y) + threadIdx.y;
	int depth = ( blockDim.z * blockIdx.z) + threadIdx.z;

	int vertex = (column + (row * blockDim.x * gridDim.x)) + (depth * blockDim.x * gridDim.x * blockDim.y * gridDim.y);
	vertex *= 15;
	int triangle = vertex;

	int x = column + pos.x * MARCHING_BLOCK_SIZE;
	int y = row + pos.y * MARCHING_BLOCK_SIZE;
	int z = depth + pos.z * MARCHING_BLOCK_SIZE;

	float points[8];
	int bitmap = 0;

	points[0] = DensityWithFloor( make_int3( x, y, z ),  0.03f );
	points[1] = DensityWithFloor( make_int3( x+1, y, z ), 0.03f );
	points[2] = DensityWithFloor( make_int3( x+1, y+1, z ), 0.03f);
	points[3] = DensityWithFloor( make_int3( x, y+1, z ), 0.03f );
	points[4] = DensityWithFloor( make_int3( x, y, z+1 ), 0.03f);
	points[5] = DensityWithFloor( make_int3( x+1, y, z+1) , 0.03f);
	points[6] = DensityWithFloor( make_int3( x+1, y+1, z+1 ),0.03f);;
	points[7] = DensityWithFloor( make_int3( x, y+1, z+1 ), 0.03f);	

	for(int i = 0; i < 15; i++)
	{
		aVertList[vertex + i] = make_float3(0); 
		aNormList[vertex + i] = make_float3(0);
		aIndexList[triangle+ i] = 0;
	}


	//--Create lookup bitmap to find the edge table
	for (int i = 0; i < 8; i++)
	{
		//points[i] -= column * 0.1;
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
		//-- point cube
		float3 pCube[8];
		pCube[0] = make_float3(0, 0, 0);
		pCube[1] = make_float3(1, 0, 0);
		pCube[2] = make_float3(1, 1, 0);
		pCube[3] = make_float3(0, 1, 0);
		pCube[4] = make_float3(0, 0, 1);
		pCube[5] = make_float3(1, 0, 1);
		pCube[6] = make_float3(1, 1, 1);
		pCube[7] = make_float3(0, 1, 1);
		

	float3 vertlist[12];

    vertlist[0] = InterpVertexPos( pCube[0], pCube[1], points[0], points[1]);
    vertlist[1] = InterpVertexPos( pCube[1], pCube[2], points[1], points[2]);
    vertlist[2] = InterpVertexPos( pCube[2], pCube[3], points[2], points[3]);
    vertlist[3] = InterpVertexPos( pCube[3], pCube[0], points[3], points[0]);

	vertlist[4] = InterpVertexPos( pCube[4], pCube[5], points[4], points[5]);
    vertlist[5] = InterpVertexPos( pCube[5], pCube[6], points[5], points[6]);
    vertlist[6] = InterpVertexPos( pCube[6], pCube[7], points[6], points[7]);
    vertlist[7] = InterpVertexPos( pCube[7], pCube[4], points[7], points[4]);

	vertlist[8] = InterpVertexPos( pCube[0], pCube[4], points[0], points[4]);
    vertlist[9] = InterpVertexPos( pCube[1], pCube[5], points[1], points[5]);
    vertlist[10] = InterpVertexPos( pCube[2], pCube[6], points[2], points[6]);
    vertlist[11] = InterpVertexPos( pCube[3], pCube[7], points[3], points[7]);

		for (int i = 0; i < vertNr; i+=3)
		{
			int dst = vertex + i;
			aVertList[dst] = vertlist[tex1Dfetch(tTriTex, (bitmap * 16) + i +2)];
			aVertList[dst] += make_float3(x,y,z);
			aIndexList[dst] = dst;

			aVertList[dst+1] = vertlist[tex1Dfetch(tTriTex, (bitmap * 16) + i + 1)];
			aVertList[dst+1] += make_float3(x,y,z);
			aIndexList[dst+1] = dst+1;

			aVertList[dst+2] = vertlist[tex1Dfetch(tTriTex, (bitmap * 16) + i + 0)];
			aVertList[dst+2] += make_float3(x,y,z);
			aIndexList[dst+2] = dst+2;

		}
	}

}

__global__ void cuda_generateNormals(float3* aVertList, float3* aNormList, unsigned int* aIndexList)
{
	int column = ( blockDim.x * blockIdx.x) + threadIdx.x;
	int row = ( blockDim.y * blockIdx.y) + threadIdx.y;
	int depth = ( blockDim.z * blockIdx.z) + threadIdx.z;

	int vertex = (column + (row * blockDim.x * gridDim.x)) + (depth * blockDim.x * gridDim.x * blockDim.y * gridDim.y);
	vertex *= 15;
	int triangle = vertex;

	//-- Generate normals
	float3 vec1, vec2, vec3;
	float3 normal;
	
	for (int i = vertex; i < vertex+15; i+=3)
	{	
		if(aIndexList[i] == 0 && aIndexList[i+1] == 0 && aIndexList[i+2] ==0)
		{continue;}

		vec1 = aVertList[i];
		vec2 = aVertList[i+1];
		vec3 = aVertList[i+2];

		float3 edge1 = vec2 - vec1;
		float3 edge2 = vec3 - vec1;
		normal = cross(edge1, edge2);
		normal = normalize(normal);

		aNormList[i] = normal;
		aNormList[i+1] = normal;
		aNormList[i+2] = normal;
	}
}


extern "C"
void launch_CreateCube(dim3 grid, dim3 threads, float3 aPos, float3* aVertList, float3* aNormList, unsigned int* aIndexList)
{
	//Here you normally run:
	
	cuda_CreateCube<<<grid, threads>>>( aPos, aVertList, aNormList, aIndexList);
	cutilCheckMsg("cuda_CreateCube failed");
	cuda_generateNormals<<<grid, threads>>>(aVertList, aNormList, aIndexList);
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


/////////////////////////////First cuda cube marching block
/////////////////////////////
//__global__ void cuda_CreateCube(float3 pos, float3* aVertList, float3* aNormList, unsigned int* aIndexList)
//{
//	int column = ( blockDim.x * blockIdx.x) + threadIdx.x;
//	int row = ( blockDim.y * blockIdx.y) + threadIdx.y;
//	int depth = ( blockDim.z * blockIdx.z) + threadIdx.z;
//
//	int vertex = (column + (row * blockDim.x * gridDim.x)) + (depth * blockDim.x * gridDim.x * blockDim.y * gridDim.y);
//	vertex *= 15;
//	int triangle = vertex;
//
//	int x = column + pos.x;
//	int y = row + pos.y;
//	int z = depth + pos.z;
//
//	float points[8];
//	unsigned int bitmap = 0;
//
//	points[0] = DensityWithFloor( make_uint3( y, x, z ),  0.05f );
//	points[1] = DensityWithFloor( make_uint3( y, x+1, z ), 0.05f );
//	points[2] = DensityWithFloor( make_uint3( y+1, x+1, z ), 0.05f);
//	points[3] = DensityWithFloor( make_uint3( y+1, x, z ), 0.05f );
//	points[4] = DensityWithFloor( make_uint3( y, x, z+1 ), 0.05f);
//	points[5] = DensityWithFloor( make_uint3( y, x+1, z+1) , 0.05f);
//	points[6] = DensityWithFloor( make_uint3( y+1, x+1, z+1 ),0.05f);
//	points[7] = DensityWithFloor( make_uint3( y+1, x, z+1 ), 0.05f);
//
//	for(int i = 0; i < 15; i++)
//	{
//		aVertList[vertex + i] = make_float3(0); 
//		aNormList[vertex + i] = make_float3(0);
//		aIndexList[triangle+ i] = 0;
//	}
//
//
//	//--Create lookup bitmap to find the edge table
//	for (int i = 0; i < 8; i++)
//	{
//		//points[i] -= column * 0.1;
//		if (points[i] > 0)
//		{
//			bitmap ^= 1<<i;						
//		}
//	}
//
//	//-- Creating triangles
//	unsigned int edge = tex1Dfetch(tEdgeTex, bitmap);
//	unsigned int vertNr = tex1Dfetch(tNRVertsTex ,bitmap);
//	if (vertNr >= 255 || vertNr <= 0)
//	{
//
//	}
//	else
//	{
//		float3 vertsPos[12]; 
//		//-- front
//		vertsPos[0]  = make_float3(0,		0.5,	0);
//		vertsPos[1]  = make_float3(0.5f,	1,		0);
//		vertsPos[2]  = make_float3(1,		0.5f,	0);
//		vertsPos[3]  = make_float3(0.5f,	0,		0);
//
//		//-- back
//		vertsPos[4]  = make_float3(0,		0.5f,	1);
//		vertsPos[5]  = make_float3(0.5f,	1,		1);
//		vertsPos[6]  = make_float3(1,		0.5f,	1);
//		vertsPos[7]  = make_float3(0.5f,	0,		1);
//
//		//-- mid
//		vertsPos[8]  = make_float3(0,		0,		0.5f);
//		vertsPos[9]  = make_float3(0,		1,		0.5f);
//		vertsPos[10]  = make_float3(1,		1,		0.5f);
//		vertsPos[11]  = make_float3(1,		0,		0.5f);
//	
//
//		for (int i = 0; i < vertNr; i++)
//		{
//			aVertList[vertex] = vertsPos[tex1Dfetch(tTriTex, (bitmap * 16) + i)];
//			aVertList[vertex] += make_float3( row, column, depth);
//			//aVertList[vertex] *= make_float3( 0.05, 0.05, 0.05);
//			aIndexList[triangle] = vertex++;
//			triangle++;
//		}
//	}
//
//}