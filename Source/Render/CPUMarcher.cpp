//-- OpenGL implementation headers
#include "windows.h"
#include "glew.h"
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "glut.h"
#include "freeglut_ext.h"
//-- GL End

//-- Project includes
#include "CPUMarcher.h"
#include "../Misc/Perlin.h"
#include "CPUTables.h"
#include "../Misc/Vector3.h"
#include "Geometry.h"
//-- Pro end

//-- Quick pie
#define PI				3.14159265358979323846264338327950288419716939937510582097494459072381640628620899862803482534211706798f



CPUMarcher::CPUMarcher()
{
	mPerlin = new CPerlin();
	mPerlin->Initialize(1);
}

CPUMarcher::~CPUMarcher()
{
	delete mPerlin;
}

void CPUMarcher::Destroy()
{
	delete [] mData;
}

void CPUMarcher::FillPerlin()
{
	float piDev = PI * 5;
	//-- Filling the cube
	for (int x = 1; x < mWidth; x++)
	{
		for (int y = 1; y < mHeight; y++)
		{
			for (int z = 1; z < mDepth; z++)
			{
				mData[ (x * (int)mHeight * (int)mDepth) + (y * (int)mDepth) + z ] = mPerlin->Noise3((float)x / piDev,(float)y / piDev,(float)z / piDev);
			}
		}
	}
}

void CPUMarcher::FillSphere()
{
	//-- Circle fill
	Vector3 mid = Vector3(mWidth/2, mHeight/2, mDepth/2);
	for (int x = 1; x < mWidth-1; x++)
	{
		for (int y = 1; y < mHeight-1; y++)
		{
			for (int z = 1; z < mDepth-1; z++)
			{
				Vector3 vec1 = Vector3(x,y,z) - mid;
				if (vec1.Length() < 10)
				{
					mData[ (x * (int)mHeight * (int)mDepth) + (y * (int)mDepth) + z ] = 1;
				}
				else
				{
					mData[ (x * (int)mHeight * (int)mDepth) + (y * (int)mDepth) + z ] = -1;
				}		
			}
		}
	}
}

void CPUMarcher::Init(int width, int height, int depth)
{
	mWidth = width +1;
	mHeight = height +1;
	mDepth = depth +1;

	//-- Create and clear point data
	mData = new float[mWidth * mHeight * mDepth];
	memset(mData, 0, sizeof(float) * mWidth * mDepth * mHeight);

	//-- Filling point data with stuff
	//FillPerlin();
	FillSphere();	
}


void CPUMarcher::Cubemarch()
{
	mVertCount	= mWidth*mHeight*mDepth*5;//5 being the maximum amount of triangles per voxel
	mNormCount  = mVertCount;
	mTriCount	= mWidth*mHeight*mDepth*5;

	mBlocks = new MarchBlock();
	mBlocks->VertList = new Vector3[mVertCount];
	mBlocks->NormList = new Vector3[mNormCount];
	mBlocks->TriList = new Triangle[mTriCount];
	
	Vector3* vertBuf = mBlocks->VertList;
	Triangle* triBuf = mBlocks->TriList;

	int vertUsed = 0;
	int triUsed = 0;
	float points[8];
	unsigned int bitmap = 0;
	for (int x = 1; x < mWidth-1; x++)
	{
		for (int y = 1; y < mHeight-1; y++)
		{
			for (int z = 2; z < mDepth-2; z++)
			{
				points[0] = mData[ (x * (int)mHeight * (int)mDepth) + (y * (int)mDepth) + z ];
				points[1] = mData[ (x * (int)mHeight * (int)mDepth) + ((y+1) * (int)mDepth) + z ];
				points[2] = mData[ ((x+1) * (int)mHeight * (int)mDepth) + ((y+1) * (int)mDepth) + z ];
				points[3] = mData[ ((x+1) * (int)mHeight * (int)mDepth) + (y * (int)mDepth) + z ];
				points[4] = mData[ (x * (int)mHeight * (int)mDepth) + (y * (int)mDepth) + z+1 ];
				points[5] = mData[ (x * (int)mHeight * (int)mDepth) + ((y+1) * (int)mDepth) + z+1 ];
				points[6] = mData[ ((x+1) * (int)mHeight * (int)mDepth) + ((y+1) * (int)mDepth) + z+1 ];
				points[7] = mData[ ((x+1) * (int)mHeight * (int)mDepth) + (y * (int)mDepth) + z+1 ];

				//--Create lookup bitmap to find the edge table
				bitmap = 0;
				for (int i = 0; i < 8; i++)
				{
					if (points[i] > 0)
					{
						bitmap ^= 1<<i;						
					}
				}

				//-- Creating triangles
				unsigned int edge = edgeTablePC[bitmap];
				unsigned int vertNr = numVertsTablePC[bitmap];
				if (vertNr >= 255 || vertNr <= 0)
				{
					continue;
				}

				for (int i = 0; i < vertNr/3; i++)
				{
					int vert  = i*3;
					vertBuf[vertUsed].x = vertsPosPC[triTablePC[bitmap][vert]][0] + (float)x;
					vertBuf[vertUsed].y = vertsPosPC[triTablePC[bitmap][vert]][1] + (float)y;
					vertBuf[vertUsed].z = vertsPosPC[triTablePC[bitmap][vert]][2] + (float)z;
					triBuf[triUsed].one = vertUsed++;
					
					vert++;
					vertBuf[vertUsed].x = vertsPosPC[triTablePC[bitmap][vert]][0] + (float)x;
					vertBuf[vertUsed].y = vertsPosPC[triTablePC[bitmap][vert]][1] + (float)y;
					vertBuf[vertUsed].z = vertsPosPC[triTablePC[bitmap][vert]][2] + (float)z;
					triBuf[triUsed].two = vertUsed++;	

					vert++;
					vertBuf[vertUsed].x = vertsPosPC[triTablePC[bitmap][vert]][0] + (float)x;
					vertBuf[vertUsed].y = vertsPosPC[triTablePC[bitmap][vert]][1] + (float)y;
					vertBuf[vertUsed].z = vertsPosPC[triTablePC[bitmap][vert]][2] + (float)z;
					triBuf[triUsed].three = vertUsed++;
					triUsed++;
				}			
			}
		}
	}

	mBlocks->TriFilled = triUsed;
	mBlocks->VertFilled = vertUsed;
	_CalculateNormals(mBlocks);
}

void CPUMarcher::_CalculateNormals(MarchBlock* aBlock)
{
	//-- Generate normals
	Vector3 vec1, vec2, vec3;
	Vector3 normal;
	Vector3* normBuf = aBlock->NormList;
	for (int i = 0; i < aBlock->VertFilled; i+=3)
	{	
		vec1 = aBlock->VertList[i];// Vector3(mVertList[i],mVertList[i+1],mVertList[i+2]);
		vec2 = aBlock->VertList[i+1];
		vec3 = aBlock->VertList[i+2];

		Vector3 edge1 = vec2 - vec1;
		Vector3 edge2 = vec3 - vec1;
		normal = edge1.Cross(edge2);
		normal.Normalize();

		normBuf[i] = normal;
		normBuf[i+1] = normal;
		normBuf[i+2] = normal;
	}
}

void CPUMarcher::Render()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//glLoadIdentity();


	//-- render points
	glPointSize(15);
	glColor3f(1,1,1);

	glDisable(GL_LIGHTING);
	glBegin(GL_POINTS);
	{
		for (int x = 0; x < mWidth; x++)
		{
			for (int y = 0; y < mHeight; y++)
			{
				for (int z = 0; z < mWidth; z++)
				{
					float color = mData[ (x * (int)mHeight * (int)mDepth) + (y * (int)mDepth) + z ];
					glColor3f(color, color, color);
					glVertex3f( (x - (mWidth/2)) / 10, (y - (mHeight/2))/ 10, (-z - (mDepth/2))/ 10);
				}
			}
		}
	}
	glEnd();

	glEnable(GL_LIGHTING);
	//glScalef(0.05f,0.05f,0.05f);
	glColor3f(1,1,0);
	//-- Render polies
	Vector3* vertList = mBlocks->VertList;
	Vector3* normList = mBlocks->NormList;
	Triangle* triList = mBlocks->TriList;
	glBegin(GL_TRIANGLES);
	{
		for (int i = 0; i < mBlocks->TriFilled; i++)
		{
			glNormal3fv(normList[triList[i].one].cell);
			glVertex3f(vertList[triList[i].one].x*0.05f, vertList[triList[i].one].y*0.05f, vertList[triList[i].one].z*0.05f);
			glNormal3fv(normList[triList[i].two].cell);
			glVertex3f(vertList[triList[i].two].x*0.05f, vertList[triList[i].two].y*0.05f, vertList[triList[i].two].z*0.05f);
			glNormal3fv(normList[triList[i].three].cell);
			glVertex3f(vertList[triList[i].three].x*0.05f, vertList[triList[i].three].y*0.05f, vertList[triList[i].three].z*0.05f);
		}
	}
	glEnd();

	glPopMatrix();
}