#include "CUDABlock.h"

#include "glew.h"

#include <cutil_inline.h>    // includes cuda.h and cuda_runtime_api.h
#include <cutil_gl_inline.h>

#include <cuda_gl_interop.h>

extern "C" void launch_CreateCube(GenerateInfo* agInfo, dim3 grid, dim3 threads, float3 aPos, float3* aVertList, float3* aNormList, GLuint* aIndexList);
extern "C" void host_CreatePerlinData(GenerateInfo* agInfo, dim3 grid, dim3 threads, float3 pos, int rank);

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
	unsigned int MaxVertices = m_Rank * m_Rank * m_Rank * 15;
	ResizeVBOs(MaxVertices, MaxVertices);

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
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Vertices );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(float3) * vertices, 0, GL_DYNAMIC_DRAW_ARB );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Normals );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(float3) * vertices, 0, GL_DYNAMIC_DRAW_ARB );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Indices );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(GLuint) * indices, 0, GL_DYNAMIC_DRAW_ARB );
}


void CUDABlock::Build(GenerateInfo* agInfo)
{
	m_FaceCount= (32*32*32*15); //TODO: change propperly

	//Generate perlin textures
	dim3 PerlingridDim(17,17,1);
	dim3 PerlinblockDim(2,2,34);
	host_CreatePerlinData(agInfo, PerlingridDim, PerlinblockDim, mPos, 33);

	//Count vertices

	//Resize VBO's
	ResizeVBOs(m_FaceCount, m_FaceCount);

	//Map ALL THE VBO'S
	size_t num_bytes;
	cutilSafeCall(cudaGraphicsMapResources(1, &cuda_VBO_Vertices, 0));
	cutilSafeCall(cudaGraphicsResourceGetMappedPointer((void**)&cuda_Vertices, &num_bytes, cuda_VBO_Vertices));
	cutilSafeCall(cudaGraphicsMapResources(1, &cuda_VBO_Normals, 0));
	cutilSafeCall(cudaGraphicsResourceGetMappedPointer((void**)&cuda_Normals, &num_bytes, cuda_VBO_Normals));
	cutilSafeCall(cudaGraphicsMapResources(1, &cuda_VBO_Indices, 0));
	cutilSafeCall(cudaGraphicsResourceGetMappedPointer((void**)&cuda_Indices, &num_bytes, cuda_VBO_Indices));

	//Create cube
	dim3 gridDim(16,16,1);
	dim3 blockDim(2,2,32);
	launch_CreateCube(agInfo, gridDim, blockDim, mPos, cuda_Vertices, cuda_Normals, cuda_Indices);

	//Unmap
	cutilSafeCall(cudaGraphicsUnmapResources(1, &cuda_VBO_Vertices, 0));
	cutilSafeCall(cudaGraphicsUnmapResources(1, &cuda_VBO_Normals, 0));
	cutilSafeCall(cudaGraphicsUnmapResources(1, &cuda_VBO_Indices, 0));
}

#define BUFFER_OFFSET(i) ((char*)0 + (i))
void CUDABlock::Render()
{
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