#include <cutil_inline.h>
#include <cuda_runtime_api.h>
#include <thrust/device_vector.h>
#include <thrust/scan.h>
#include "cutil_math.h"
#include "../Render/CUDATables.h"
#include "../Render/Defines.h"

// Data
#define SIZE 256
#define MASK 0xFF

//Perlin Tables GPU
unsigned char* cuda_p;
float* cuda_gx;
float* cuda_gy;
float* cuda_gz;
texture<unsigned char> tP;
texture<float> tGX;
texture<float> tGY;
texture<float> tGZ;

//-- Table textures
texture<unsigned int, 1, cudaReadModeElementType> tEdgeTex;
texture<unsigned int, 1, cudaReadModeElementType> tNRVertsTex;
texture<unsigned int, 1, cudaReadModeElementType> tTriTex;

//-- Sampling data
float* dataPerlin1;
float* dataPerlin2;
float* dataPerlin3;
int rankPerlin;
texture<unsigned char, 1, cudaReadModeNormalizedFloat> tPerlin1;
texture<unsigned char, 1, cudaReadModeNormalizedFloat> tPerlin2; //-- octave 2
texture<unsigned char, 1, cudaReadModeNormalizedFloat> tPerlin3; //-- octave 3

extern __device__ float Noise3(float x, float y, float z);

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
	cutilSafeCall(cudaBindTexture(0, tPerlin1, *aDstCuda1, cD) );

    cutilSafeCall(cudaMalloc((void**) aDstCuda2, dataSize*sizeof(float)));
    cutilSafeCall(cudaMemcpy((void*)* aDstCuda2, (void *)aData2, dataSize*sizeof(float), cudaMemcpyHostToDevice) );
    cutilSafeCall(cudaBindTexture(0, tPerlin2, *aDstCuda2, cD) );

    cutilSafeCall(cudaMalloc((void**) aDstCuda3, dataSize*sizeof(float)));
    cutilSafeCall(cudaMemcpy((void*)* aDstCuda3, (void *)aData3, dataSize*sizeof(float), cudaMemcpyHostToDevice) );
    cutilSafeCall(cudaBindTexture(0, tPerlin3, *aDstCuda2, cD) );
}

__device__ float SampleData1( int3 pos )
{
    unsigned int i = (pos.z*PERLIN_DATA_RANK*PERLIN_DATA_RANK) + (pos.y*PERLIN_DATA_RANK) + pos.x;
    return tex1Dfetch(tPerlin1, i);
}

__device__ float SampleData2( int3 pos )
{
	unsigned int i = (pos.z*PERLIN_DATA_RANK*PERLIN_DATA_RANK) + (pos.y*PERLIN_DATA_RANK) + pos.x;
    return tex1Dfetch(tPerlin2, i);
}

__device__ float SampleData3( int3 pos )
{
    unsigned int i = (pos.z*PERLIN_DATA_RANK*PERLIN_DATA_RANK) + (pos.y*PERLIN_DATA_RANK) + pos.x;
    return tex1Dfetch(tPerlin3, i);
}


__device__ float DensityWithFloor( int3 pos, float floorMultiplier)
{
	return SampleData1( pos ) + SampleData2( pos ) * 0.1f + SampleData3( pos ) * 0.05f;// - (pos.y * floorMultiplier);
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

__global__ void cuda_CreatePerlin(float3 pos, int rank, float* aPerlin1, float* aPerlin2, float* aPerlin3)
{
	#define PI				3.14159265358979323846264338327950288419716939937510582097494459072381640628620899862803482534211706798f
		float	piDev1				= PI * 5;
		float	piDev2				= PI * 1;
		float	piDev3				= PI * 0.1;
	#undef PI

	int column = ( blockDim.x * blockIdx.x) + threadIdx.x;
	int row = ( blockDim.y * blockIdx.y) + threadIdx.y;
	int depth = ( blockDim.z * blockIdx.z) + threadIdx.z;
	
	int voxel = (column + (row * blockDim.x * gridDim.x)) + (depth * blockDim.x * gridDim.x * blockDim.y * gridDim.y);
	float3 index = make_float3(column+(pos.x*MARCHING_BLOCK_SIZE), row+(pos.y*MARCHING_BLOCK_SIZE), depth+(pos.z*MARCHING_BLOCK_SIZE));
	
	aPerlin1[voxel] = Noise3(index.x / piDev1, index.y / piDev1, index.z / piDev1);
	aPerlin2[voxel] = Noise3((index.x+rank*2) / piDev2, index.y / piDev2, index.z / piDev2);
	aPerlin3[voxel] = Noise3((index.x+rank*4) / piDev3, index.y / piDev3, index.z / piDev3);
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

	points[0] = DensityWithFloor( make_int3( column,	row,	depth ),	0.03f) - ((float)y * 0.03f);
	points[1] = DensityWithFloor( make_int3( column+1,	row,	depth ),	0.03f) - ((float)y * 0.03f);
	points[2] = DensityWithFloor( make_int3( column+1,	row+1,	depth ),	0.03f) - ((float)(y+1) * 0.03f);
	points[3] = DensityWithFloor( make_int3( column,	row+1,	depth ),	0.03f) - ((float)(y+1) * 0.03f);
	points[4] = DensityWithFloor( make_int3( column,	row,	depth+1 ),	0.03f) - ((float)y * 0.03f);
	points[5] = DensityWithFloor( make_int3( column+1,	row,	depth+1) ,	0.03f) - ((float)y * 0.03f);
	points[6] = DensityWithFloor( make_int3( column+1,	row+1,	depth+1 ),	0.03f) - ((float)(y+1) * 0.03f);
	points[7] = DensityWithFloor( make_int3( column,	row+1,	depth+1 ),	0.03f) - ((float)(y+1) * 0.03f);

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

extern "C"
void host_InitPerlinData(int rank)
{
	cudaChannelFormatDesc cD = cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat);
	rankPerlin = rank;
	int dataSize = rank*rank*rank;
	
	// allocate data as textures
    cutilSafeCall(cudaMalloc((void**) &dataPerlin1, dataSize*sizeof(float)));
    cutilSafeCall(cudaMalloc((void**) &dataPerlin2, dataSize*sizeof(float)));
    cutilSafeCall(cudaMalloc((void**) &dataPerlin3, dataSize*sizeof(float)));
}

extern "C"
void host_CreatePerlinData(dim3 grid, dim3 threads, float3 pos, int rank)
{
	cudaChannelFormatDesc cD = cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat);
	
	//Unbind Textures
	cutilSafeCall(cudaUnbindTexture(tPerlin1));
	cutilSafeCall(cudaUnbindTexture(tPerlin2));
	cutilSafeCall(cudaUnbindTexture(tPerlin3));
	
	//Fill perlins
	cuda_CreatePerlin<<<grid, threads>>>(pos, rank, dataPerlin1, dataPerlin2, dataPerlin3);
       
    //Bind as texture
	cutilSafeCall(cudaBindTexture(0, tPerlin1, dataPerlin1, cD) );
	cutilSafeCall(cudaBindTexture(0, tPerlin2, dataPerlin2, cD) );
	cutilSafeCall(cudaBindTexture(0, tPerlin3, dataPerlin3, cD) );
}


//PERLIN

extern "C"
void host_PerlinInitialize(unsigned int nSeed)
{
	// Permutation table
	unsigned char p[SIZE];
	// Gradients
	float gx[SIZE];
	float gy[SIZE];
	float gz[SIZE];

	cudaChannelFormatDesc cDUnsigned = cudaCreateChannelDesc(8, 0, 0, 0, cudaChannelFormatKindUnsigned);
	cudaChannelFormatDesc cDFloat = cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat);
	int i, j, nSwap;

	srand(nSeed);
	// Initialize the permutation table
	for(i = 0; i < SIZE; i++)
		p[i] = i;

	for(i = 0; i < SIZE; i++)
	{
		j = rand() & MASK;

		nSwap = p[i];
		p[i]  = p[j];
		p[j]  = nSwap;
	}

	// Generate the gradient lookup tables
	for(i = 0; i < SIZE; i++)
	{
		// Ken Perlin proposes that the gradients are taken from the unit 
		// circle/sphere for 2D/3D, but there are no noticable difference 
		// between that and what I'm doing here. For the sake of generality 
		// I will not do that.

		gx[i] = float(rand())/(RAND_MAX/2) - 1.0f; 
		gy[i] = float(rand())/(RAND_MAX/2) - 1.0f;
		gz[i] = float(rand())/(RAND_MAX/2) - 1.0f;
	}
	
	//Alloc Tables
	cutilSafeCall(cudaMalloc((void**) &cuda_p, SIZE*sizeof(unsigned char)));
	cutilSafeCall(cudaMalloc((void**) &cuda_gx, SIZE*sizeof(float)));
	cutilSafeCall(cudaMalloc((void**) &cuda_gy, SIZE*sizeof(float)));
	cutilSafeCall(cudaMalloc((void**) &cuda_gz, SIZE*sizeof(float)));
	
	cutilSafeCall(cudaMemcpy((void*) cuda_p, (void *)p, SIZE*sizeof(unsigned char), cudaMemcpyHostToDevice) );
	cutilSafeCall(cudaMemcpy((void*) cuda_gx, (void *)gx, SIZE*sizeof(float), cudaMemcpyHostToDevice) );
	cutilSafeCall(cudaMemcpy((void*) cuda_gy, (void *)gy, SIZE*sizeof(float), cudaMemcpyHostToDevice) );
	cutilSafeCall(cudaMemcpy((void*) cuda_gz, (void *)gz, SIZE*sizeof(float), cudaMemcpyHostToDevice) );
	
	cutilSafeCall(cudaBindTexture(0, tP, cuda_p, cDUnsigned) );
	cutilSafeCall(cudaBindTexture(0, tGX, cuda_gx, cDFloat) );
	cutilSafeCall(cudaBindTexture(0, tGY, cuda_gy, cDFloat) );
	cutilSafeCall(cudaBindTexture(0, tGZ, cuda_gz, cDFloat) );
}

__device__ float Noise1(float x)
{
		// Compute what gradients to use
	int qx0 = (int)floorf(x);
	int qx1 = qx0 + 1;
	float tx0 = x - (float)qx0;
	float tx1 = tx0 - 1;

	// Make sure we don't come outside the lookup table
	qx0 = qx0 & MASK;
	qx1 = qx1 & MASK;

	// Compute the dotproduct between the vectors and the gradients
	float v0 = tex1Dfetch(tGX, qx0)*tx0;
	float v1 = tex1Dfetch(tGX, qx1)*tx1;

	// Modulate with the weight function
	float wx = (3 - 2*tx0)*tx0*tx0;
	float v = v0 - wx*(v0 - v1);

	return v;
}

__device__ float Noise2(float x, float y)
{
	// Compute what gradients to use
	int qx0 = (int)floorf(x);
	int qx1 = qx0 + 1;
	float tx0 = x - (float)qx0;
	float tx1 = tx0 - 1;

	int qy0 = (int)floorf(y);
	int qy1 = qy0 + 1;
	float ty0 = y - (float)qy0;
	float ty1 = ty0 - 1;

	// Make sure we don't come outside the lookup table
	qx0 = qx0 & MASK;
	qx1 = qx1 & MASK;

	qy0 = qy0 & MASK;
	qy1 = qy1 & MASK;

	// Permutate values to get pseudo randomly chosen gradients
	int q00 = tex1Dfetch(tP, (qy0 + tex1Dfetch(tP, qx0)) & MASK);
	int q01 = tex1Dfetch(tP, (qy0 + tex1Dfetch(tP, qx1)) & MASK);

	int q10 = tex1Dfetch(tP, (qy1 + tex1Dfetch(tP, qx0)) & MASK);
	int q11 = tex1Dfetch(tP, (qy1 + tex1Dfetch(tP, qx1)) & MASK);

	// Compute the dotproduct between the vectors and the gradients
	float v00 = tex1Dfetch(tGX, q00)*tx0 + tex1Dfetch(tGY, q00) * ty0;
	float v01 = tex1Dfetch(tGX, q01)*tx1 + tex1Dfetch(tGY, q01) * ty0;

	float v10 = tex1Dfetch(tGX, q10)*tx0 + tex1Dfetch(tGY, q10) * ty1;
	float v11 = tex1Dfetch(tGX, q11)*tx1 + tex1Dfetch(tGY, q11) * ty1;

	// Modulate with the weight function
	float wx = (3 - 2*tx0)*tx0*tx0;
	float v0 = v00 - wx*(v00 - v01);
	float v1 = v10 - wx*(v10 - v11);

	float wy = (3 - 2*ty0)*ty0*ty0;
	float v = v0 - wy*(v0 - v1);

	return v;	
}

__device__ float Noise3(float x, float y, float z)
{
	// Compute what gradients to use
	int qx0 = (int)floorf(x);
	int qx1 = qx0 + 1;
	float tx0 = x - (float)qx0;
	float tx1 = tx0 - 1;

	int qy0 = (int)floorf(y);
	int qy1 = qy0 + 1;
	float ty0 = y - (float)qy0;
	float ty1 = ty0 - 1;

	int qz0 = (int)floorf(z);
	int qz1 = qz0 + 1;
	float tz0 = z - (float)qz0;
	float tz1 = tz0 - 1;

	// Make sure we don't come outside the lookup table
	qx0 = qx0 & MASK;
	qx1 = qx1 & MASK;

	qy0 = qy0 & MASK;
	qy1 = qy1 & MASK;

	qz0 = qz0 & MASK;
	qz1 = qz1 & MASK;

	// Permutate values to get pseudo randomly chosen gradients
	int q000 = tex1Dfetch(tP, (qz0 + tex1Dfetch(tP, (qy0 + tex1Dfetch(tP, qx0)) & MASK)) & MASK);
	int q001 = tex1Dfetch(tP, (qz0 + tex1Dfetch(tP, (qy0 + tex1Dfetch(tP, qx1)) & MASK)) & MASK);

	int q010 = tex1Dfetch(tP, (qz0 + tex1Dfetch(tP, (qy1 + tex1Dfetch(tP, qx0)) & MASK)) & MASK);
	int q011 = tex1Dfetch(tP, (qz0 + tex1Dfetch(tP, (qy1 + tex1Dfetch(tP, qx1)) & MASK)) & MASK);

	int q100 = tex1Dfetch(tP, (qz1 + tex1Dfetch(tP, (qy0 + tex1Dfetch(tP, qx0)) & MASK)) & MASK);
	int q101 = tex1Dfetch(tP, (qz1 + tex1Dfetch(tP, (qy0 + tex1Dfetch(tP, qx1)) & MASK)) & MASK);

	int q110 = tex1Dfetch(tP, (qz1 + tex1Dfetch(tP, (qy1 + tex1Dfetch(tP, qx0)) & MASK)) & MASK);
	int q111 = tex1Dfetch(tP, (qz1 + tex1Dfetch(tP, (qy1 + tex1Dfetch(tP, qx1)) & MASK)) & MASK);

	// Compute the dotproduct between the vectors and the gradients
	float v000 = tex1Dfetch(tGX, q000)*tx0 + tex1Dfetch(tGY, q000)*ty0 + tex1Dfetch(tGZ, q000)*tz0;
	float v001 = tex1Dfetch(tGX, q001)*tx1 + tex1Dfetch(tGY, q001)*ty0 + tex1Dfetch(tGZ, q001)*tz0;

	float v010 = tex1Dfetch(tGX, q010)*tx0 + tex1Dfetch(tGY, q010)*ty1 + tex1Dfetch(tGZ, q010)*tz0;
	float v011 = tex1Dfetch(tGX, q011)*tx1 + tex1Dfetch(tGY, q011)*ty1 + tex1Dfetch(tGZ, q011)*tz0;

	float v100 = tex1Dfetch(tGX, q100)*tx0 + tex1Dfetch(tGY, q100)*ty0 + tex1Dfetch(tGZ, q100)*tz1;
	float v101 = tex1Dfetch(tGX, q101)*tx1 + tex1Dfetch(tGY, q101)*ty0 + tex1Dfetch(tGZ, q101)*tz1;

	float v110 = tex1Dfetch(tGX, q110)*tx0 + tex1Dfetch(tGY, q110)*ty1 + tex1Dfetch(tGZ, q110)*tz1;
	float v111 = tex1Dfetch(tGX, q111)*tx1 + tex1Dfetch(tGY, q111)*ty1 + tex1Dfetch(tGZ, q111)*tz1;

	// Modulate with the weight function
	float wx = (3 - 2*tx0)*tx0*tx0;
	float v00 = v000 - wx*(v000 - v001);
	float v01 = v010 - wx*(v010 - v011);
	float v10 = v100 - wx*(v100 - v101);
	float v11 = v110 - wx*(v110 - v111);

	float wy = (3 - 2*ty0)*ty0*ty0;
	float v0 = v00 - wy*(v00 - v01);
	float v1 = v10 - wy*(v10 - v11);

	float wz = (3 - 2*tz0)*tz0*tz0;
	float v = v0 - wz*(v0 - v1);

	return v;	
}
