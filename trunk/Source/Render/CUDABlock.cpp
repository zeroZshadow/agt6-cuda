#include "CUDABlock.h"

#include "glew.h"

#include <cutil_inline.h>    // includes cuda.h and cuda_runtime_api.h
#include <cutil_gl_inline.h>

#include <cuda_gl_interop.h>
#include "Defines.h"

extern "C" void launch_CreateCube(GenerateInfo* agInfo, dim3 grid, dim3 threads, float3 aPos, float3* aVertList, float3* aNormList, GLuint* aIndexList);
extern "C" void host_CreatePerlinData(GenerateInfo* agInfo, dim3 grid, dim3 threads, float3 pos, int rank);

extern "C" void launch_ClassifyVoxel( dim3 grid, dim3 threads, GenerateInfo agInfo, float3 pos, 
						  uint* voxelVertCnt, uint *voxelOccupied);

extern "C" void launch_compactVoxels(dim3 grid, dim3 threads, uint *compactedVoxelArray, uint *voxelOccupied, uint *voxelOccupiedScan, uint numVoxels);

extern "C" void launch_generateTriangles(dim3 grid, dim3 threads, GenerateInfo agInfo, float3 pos, 
										 void *aDataList, uint *compactedV, uint *numVertsScanned, 
										 uint activeVoxels, uint maxVerts);

extern "C" void ThrustScanWrapper(unsigned int* output, unsigned int* input, unsigned int numElements);



uint* CUDABlock::cuda_voxelVerts = 0;
uint* CUDABlock::cuda_voxelVertsScan =0;
uint* CUDABlock::cuda_voxelOccupied =0;
uint* CUDABlock::cuda_voxelOccupiedScan =0;
uint* CUDABlock::cuda_compVoxelArray =0;

CUDABlock::CUDABlock()
{
	m_VBO_Data = 0;
	cuda_VBO_Data = 0;
	cuda_Data = 0;

	m_IndexCount = 0;
	m_VertexCount = 0;

	m_Rank = 32;

	mPos.x = 0;
	mPos.x = 0;
	mPos.x = 0;
}

CUDABlock::~CUDABlock()
{
	cutilSafeCall(cudaGraphicsUnregisterResource(cuda_VBO_Data));

	glDeleteBuffers(1, &m_VBO_Data);

	m_VBO_Data = 0;
	cuda_VBO_Data = 0;
	cuda_Data = 0;

	m_IndexCount = 0;
}

void CUDABlock::Init(float x, float y, float z)
{
	mPos.x = x;
	mPos.y = y;
	mPos.z = z;

	//Create VBO's
	glGenBuffersARB( 1, &m_VBO_Data );

	//Setup VBO's
	m_IndexCount = 0;
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Data );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(float3) * 2 + sizeof(uint) * 3, 0, GL_STATIC_DRAW_ARB );

	if(glGetError() != GL_NO_ERROR)	{
		printf("Error creating VBOs");
		return;
	}
	cutilSafeCall(cudaGraphicsGLRegisterBuffer(&cuda_VBO_Data, m_VBO_Data, cudaGraphicsMapFlagsWriteDiscard));
}

bool CUDABlock::ResizeVBOs(int vertices, int indices)
{
	m_IndexCount = indices;

	//Unregister
	cutilSafeCall(cudaGraphicsUnregisterResource(cuda_VBO_Data));

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Data );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, (sizeof(float3) * vertices * 2) + (sizeof(uint) * indices), 0, GL_STATIC_DRAW_ARB );

	if(glGetError() == GL_OUT_OF_MEMORY)
	{
		printf("> Ran out of memory\n");
		m_IndexCount = 0;
		m_VertexCount = 0;
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Data );
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(float3) * 2 + sizeof(uint) * 3, 0, GL_STATIC_DRAW_ARB );
	}

	//Register
	if(cudaGraphicsGLRegisterBuffer(&cuda_VBO_Data, m_VBO_Data, cudaGraphicsMapFlagsWriteDiscard) == cudaErrorMemoryAllocation) {m_IndexCount = 0; return false;}
	return true;
}


bool CUDABlock::Build(GenerateInfo* agInfo)
{
	//Generate perlin textures
	dim3 PerlingridDim(17,17,1);
	dim3 PerlinblockDim(2,2,34);
	host_CreatePerlinData(agInfo, PerlingridDim, PerlinblockDim, mPos, 33);


	dim3 gridDim(16,16,1);
	dim3 blockDim(2,2,32);
	launch_ClassifyVoxel( gridDim, blockDim, *agInfo, mPos, cuda_voxelVerts, cuda_voxelOccupied);

	ThrustScanWrapper(cuda_voxelOccupiedScan, cuda_voxelOccupied, MARCHING_BLOCK_SIZE_POWER3);

	uint lastElement, lastScanElement;
	cutilSafeCall(cudaMemcpy((void *) &lastElement, 
		(void *) (cuda_voxelOccupied + MARCHING_BLOCK_SIZE_POWER3-1), 
		sizeof(uint), cudaMemcpyDeviceToHost));
	cutilSafeCall(cudaMemcpy((void *) &lastScanElement, 
		(void *) (cuda_voxelOccupiedScan + MARCHING_BLOCK_SIZE_POWER3-1), 
		sizeof(uint), cudaMemcpyDeviceToHost));
	uint activeVoxels = lastElement + lastScanElement;

	if (activeVoxels==0) {
		// return if there are no full voxels
		return ResizeVBOs(1, 3);
	}

	int threads = 128;
	dim3 grid(MARCHING_BLOCK_SIZE_POWER3 / threads, 1, 1);
	// get around maximum grid size of 65535 in each dimension
	if (grid.x > 65535) {
		grid.y = grid.x / 32768;
		grid.x = 32768;
	}
	launch_compactVoxels(grid, threads, cuda_compVoxelArray,
		cuda_voxelOccupied, cuda_voxelOccupiedScan, MARCHING_BLOCK_SIZE_POWER3);


	ThrustScanWrapper(cuda_voxelVertsScan, cuda_voxelVerts, MARCHING_BLOCK_SIZE_POWER3);

	uint lastElement2, lastScanElement2;
	cutilSafeCall(cudaMemcpy((void *) &lastElement2, (void *) (cuda_voxelVerts + MARCHING_BLOCK_SIZE_POWER3-1), sizeof(uint), cudaMemcpyDeviceToHost));
	cutilSafeCall(cudaMemcpy((void *) &lastScanElement2, (void *) (cuda_voxelVertsScan + MARCHING_BLOCK_SIZE_POWER3-1), sizeof(uint), cudaMemcpyDeviceToHost));
	m_VertexCount = lastElement2 + lastScanElement2;

	//Resize VBO's
	if(!ResizeVBOs(m_VertexCount, m_VertexCount)){
		m_IndexCount = 0;
		m_VertexCount = 0;
		return false;
	}

	//Map ALL THE VBO'S
	cutilSafeCall(cudaGraphicsMapResources(1, &cuda_VBO_Data, 0));
	cutilSafeCall(cudaGraphicsResourceGetMappedPointer((void**)&cuda_Data, &m_BytesUsed, cuda_VBO_Data));

	//launch_generateTriangles(
	dim3 grid2((int) ceil(activeVoxels / (float) 128), 1, 1);
	while(grid2.x > 65535) {
		grid2.x/=2;
		grid2.y*=2;
	}

	launch_generateTriangles(grid2, 128, *agInfo, mPos, 
		cuda_Data, 
		cuda_compVoxelArray, 
		cuda_voxelVertsScan, activeVoxels, 
		m_VertexCount);



	//Unmap
	cutilSafeCall(cudaGraphicsUnmapResources(1, &cuda_VBO_Data, 0));

	return true;
}

#define BUFFER_OFFSET(i) ((char*)0 + (i))
void CUDABlock::Render()
{
	if(m_IndexCount<=0) return;
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );

	//Vertices
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO_Data );
	glVertexPointer( 3, GL_FLOAT, 0, 0 );

	//Normals
	glNormalPointer( GL_FLOAT, 0, BUFFER_OFFSET(m_VertexCount * sizeof(float3)) );

	//Draw
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_VBO_Data );
	glDrawElements( GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, BUFFER_OFFSET(m_VertexCount * sizeof(float3) * 2) );

	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}