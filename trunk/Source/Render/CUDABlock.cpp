#include "CUDABlock.h"

#include "glew.h"

#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

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

	m_X = 0;
	m_Y = 0;
	m_Z = 0;

}

CUDABlock::~CUDABlock()
{
	cudaGraphicsUnregisterResource(cuda_VBO_Vertices);
	cudaGraphicsUnregisterResource(cuda_VBO_Normals);
	cudaGraphicsUnregisterResource(cuda_VBO_Indices);

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

void CUDABlock::Init()
{
	//Create VBO's
	glGenBuffersARB( 1, &m_VBO_Vertices );
	glGenBuffersARB( 1, &m_VBO_Normals );
	glGenBuffersARB( 1, &m_VBO_Indices );

	//Setup VBO's
	unsigned int MaxVertices = m_Rank * m_Rank * m_Rank * 5;
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Vertices );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(Vector3) * MaxVertices, NULL, GL_DYNAMIC_DRAW_ARB );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Normals );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(Vector3) * MaxVertices, NULL, GL_DYNAMIC_DRAW_ARB );

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO_Indices );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(GLuint) * MaxVertices, NULL, GL_DYNAMIC_DRAW_ARB );

	//Link VBO's
	cudaGraphicsGLRegisterBuffer(&cuda_VBO_Vertices, m_VBO_Vertices, cudaGraphicsMapFlagsWriteDiscard);
	cudaGraphicsGLRegisterBuffer(&cuda_VBO_Normals, m_VBO_Normals, cudaGraphicsMapFlagsWriteDiscard);
	cudaGraphicsGLRegisterBuffer(&cuda_VBO_Indices, m_VBO_Indices, cudaGraphicsMapFlagsWriteDiscard);
}

void CUDABlock::Build()
{
	//Map ALL THE VBO'S
	size_t num_bytes;
	cudaGraphicsMapResources(1, &cuda_VBO_Vertices, 0);
	cudaGraphicsResourceGetMappedPointer((void**)&cuda_Vertices, &num_bytes, cuda_VBO_Vertices);

	//Unmap
	cudaGraphicsUnmapResources(1, &cuda_VBO_Vertices, 0);
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
	glDrawElements( GL_TRIANGLES, m_FaceCount*3, GL_UNSIGNED_INT, BUFFER_OFFSET(0) );

	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}