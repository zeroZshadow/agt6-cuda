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
						 float3 *aVertList, float3* aNormList, 
						 uint* aTriList, uint *compactedV, uint *numVertsScanned, 
						 uint activeVoxels, uint maxVerts);

extern "C" void ThrustScanWrapper(unsigned int* output, unsigned int* input, unsigned int numElements);



uint* CUDABlock::cuda_voxelVerts = 0;
uint* CUDABlock::cuda_voxelVertsScan =0;
uint* CUDABlock::cuda_voxelOccupied =0;
uint* CUDABlock::cuda_voxelOccupiedScan =0;
uint* CUDABlock::cuda_compVoxelArray =0;

CUDABlock::CUDABlock()
{
	m_VBO_Vertices = 0;
	m_VBO_Normals = 0;
	m_VBO_Indices = 0;

	cuda_VBO_Vertices = 0;
	cuda_VBO_Normals = 0;
	cuda_VBO_Indices = 0;

	cuda_Vertices = 0;
	cuda_Normals = 0;
	cuda_Indices = 0;

	m_FaceCount = 0;

	m_Rank = 32;

	mPos.x = 0;
	mPos.x = 0;
	mPos.x = 0;
}

CUDABlock::~CUDABlock()
{
	cutilSafeCall(cudaGraphicsUnregisterResource(cuda_VBO_Vertices));
	cutilSafeCall(cudaGraphicsUnregisterResource(cuda_VBO_Normals));
	cutilSafeCall(cudaGraphicsUnregisterResource(cuda_VBO_Indices));

	glDeleteBuffers(1, &m_VBO_Vertices);
	glDeleteBuffers(1, &m_VBO_Normals);
	glDeleteBuffers(1, &m_VBO_Indices);

	m_VBO_Vertices = 0;
	m_VBO_Normals = 0;
	m_VBO_Indices = 0;

	cuda_VBO_Vertices = 0;
	cuda_VBO_Normals = 0;
	cuda_VBO_Indices = 0;

	cuda_Vertices = 0;
	cuda_Normals = 0;
	cuda_Indices = 0;

	m_FaceCount = 0;
}

void CUDABlock::Init(float x, float y, float z)
{
	mPos.x = x;
	mPos.y = y;
	mPos.z = z;

	//Create VBO's
	glGenBuffersARB( 1, &m_VBO_Vertices );
	glGenBuffersARB( 1, &m_VBO_Normals );
	glGenBuffersARB( 1, &m_VBO_Indices );

	//Setup VBO's
	m_FaceCount = 0;
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Vertices );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(float3) * 1, 0, GL_DYNAMIC_DRAW_ARB );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Normals );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(float3) * 1, 0, GL_DYNAMIC_DRAW_ARB );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Indices );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(GLuint) * 1, 0, GL_DYNAMIC_DRAW_ARB );

	if(glGetError() != GL_NO_ERROR)	{
		printf("Error creating VBOs");
		return;
	}

	//Link VBO's
	cutilSafeCall(cudaGraphicsGLRegisterBuffer(&cuda_VBO_Vertices, m_VBO_Vertices, cudaGraphicsMapFlagsWriteDiscard));
	cutilSafeCall(cudaGraphicsGLRegisterBuffer(&cuda_VBO_Normals, m_VBO_Normals, cudaGraphicsMapFlagsWriteDiscard));
	cutilSafeCall(cudaGraphicsGLRegisterBuffer(&cuda_VBO_Indices, m_VBO_Indices, cudaGraphicsMapFlagsWriteDiscard));
}

void CUDABlock::ResizeVBOs(int vertices, int indices)
{
	m_FaceCount = indices;

	//Unregister
	cutilSafeCall(cudaGraphicsUnregisterResource(cuda_VBO_Vertices));
	cutilSafeCall(cudaGraphicsUnregisterResource(cuda_VBO_Normals));
	cutilSafeCall(cudaGraphicsUnregisterResource(cuda_VBO_Indices));

	//Resize
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Vertices );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(float3) * vertices, 0, GL_DYNAMIC_DRAW_ARB );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Normals );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(float3) * vertices, 0, GL_DYNAMIC_DRAW_ARB );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Indices );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(GLuint) * vertices, 0, GL_DYNAMIC_DRAW_ARB );

	//Register
	cutilSafeCall(cudaGraphicsGLRegisterBuffer(&cuda_VBO_Vertices, m_VBO_Vertices, cudaGraphicsMapFlagsWriteDiscard));
	cutilSafeCall(cudaGraphicsGLRegisterBuffer(&cuda_VBO_Normals, m_VBO_Normals, cudaGraphicsMapFlagsWriteDiscard));
	cutilSafeCall(cudaGraphicsGLRegisterBuffer(&cuda_VBO_Indices, m_VBO_Indices, cudaGraphicsMapFlagsWriteDiscard));
}


void CUDABlock::Build(GenerateInfo* agInfo)
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
		uint totalVerts = 0;
		ResizeVBOs(1, 0);
		return;
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
	cutilSafeCall(cudaMemcpy((void *) &lastElement2, 
		(void *) (cuda_voxelVerts + MARCHING_BLOCK_SIZE_POWER3-1), 
		sizeof(uint), cudaMemcpyDeviceToHost));
	cutilSafeCall(cudaMemcpy((void *) &lastScanElement2, 
		(void *) (cuda_voxelVertsScan + MARCHING_BLOCK_SIZE_POWER3-1), 
		sizeof(uint), cudaMemcpyDeviceToHost));
	uint totalVerts = lastElement2 + lastScanElement2;

	

	//Resize VBO's
	ResizeVBOs(totalVerts, totalVerts);

	//Map ALL THE VBO'S
	size_t num_bytes;
	cutilSafeCall(cudaGraphicsMapResources(1, &cuda_VBO_Vertices, 0));
	cutilSafeCall(cudaGraphicsResourceGetMappedPointer((void**)&cuda_Vertices, &num_bytes, cuda_VBO_Vertices));
	cutilSafeCall(cudaGraphicsMapResources(1, &cuda_VBO_Normals, 0));
	cutilSafeCall(cudaGraphicsResourceGetMappedPointer((void**)&cuda_Normals, &num_bytes, cuda_VBO_Normals));
	cutilSafeCall(cudaGraphicsMapResources(1, &cuda_VBO_Indices, 0));
	cutilSafeCall(cudaGraphicsResourceGetMappedPointer((void**)&cuda_Indices, &num_bytes, cuda_VBO_Indices));

	//Create cube
	//dim3 gridDim(16,16,1);
	//dim3 blockDim(2,2,32);
	//launch_CreateCube(agInfo, gridDim, blockDim, mPos, cuda_Vertices, cuda_Normals, cuda_Indices);

	//launch_generateTriangles(
	dim3 grid2((int) ceil(activeVoxels / (float) 128), 1, 1);
	while(grid2.x > 65535) {
		grid2.x/=2;
		grid2.y*=2;
	}

	launch_generateTriangles(grid2, 128, *agInfo, mPos, 
		cuda_Vertices, cuda_Normals, cuda_Indices, 
		cuda_compVoxelArray, 
		cuda_voxelVertsScan, activeVoxels, 
		totalVerts);



	//Unmap
	cutilSafeCall(cudaGraphicsUnmapResources(1, &cuda_VBO_Vertices, 0));
	cutilSafeCall(cudaGraphicsUnmapResources(1, &cuda_VBO_Normals, 0));
	cutilSafeCall(cudaGraphicsUnmapResources(1, &cuda_VBO_Indices, 0));
}

#define BUFFER_OFFSET(i) ((char*)0 + (i))
void CUDABlock::Render()
{
	if(m_FaceCount<=0) return;
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );

	//Vertices
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO_Vertices );
	glVertexPointer( 3, GL_FLOAT, 0, 0 );

	//Normals
	glBindBuffer( GL_ARRAY_BUFFER, m_VBO_Normals );
	glNormalPointer( GL_FLOAT, 0, BUFFER_OFFSET(0) );

	//Draw
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_VBO_Indices );
	glDrawElements( GL_TRIANGLES, m_FaceCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0) );

	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}