//-- OpenGL implementation headers
#include "windows.h"
#include "glew.h"
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "glut.h"
#include "freeglut_ext.h"
//-- GL End

//-- Project includes
#include "CPUTerrain.h"
#include "../Misc/Perlin.h"
#include "CPUTables.h"
#include "../Misc/Vector3.h"
//-- Pro end

//-- Quick pie
#define PI				3.14159265358979323846264338327950288419716939937510582097494459072381640628620899862803482534211706798f



CPUTerrain::CPUTerrain()
{
	mPerlin = new CPerlin();
	mPerlin->Initialize(1);
}

CPUTerrain::~CPUTerrain()
{
	delete mPerlin;
}

void CPUTerrain::Destroy()
{
	delete [] mData;
}

void CPUTerrain::Build(int width, int height, int depth)
{
	mWidth = width +1;
	mHeight = height +1;
	mDepth = depth +1;
	mData = new float[mWidth * mHeight * mDepth];

	float piDev = PI * 5;

	//-- blanking the borders
	for (int i = 0; i < mWidth * mHeight * mWidth; i++)
	{
		mData[i] = 0;
	}

	//-- Filling the cube
	for (int x = 1; x < mWidth-1; x++)
	{
		for (int y = 1; y < mHeight-1; y++)
		{
			for (int z = 1; z < mDepth-1; z++)
			{
				mData[ (x * (int)mHeight * (int)mDepth) + (y * (int)mDepth) + z ] = mPerlin->Noise3((float)x / piDev,(float)y / piDev,(float)z / piDev);
			}
		}
	}

	mVertUsed	= 0;
	mTriUsed	= 0;
	mVertCount	= 32*32*32*3*5;
	mVertList	= new float[mVertCount];
	mNormCount  = mVertCount;
	mNormUsed	= 0;
	mNormList	= new float[mNormCount];
	mTriCount	= 32*32*32*5;
	mTriList	= new int[mTriCount];
	
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

				//--Create bitmap
				bitmap = 0;
				for (int i = 0; i < 8; i++)
				{
					if (points[i] > 0)
					{
						bitmap ^= 1<<i;						
					}
				}

				//-- Creating triangles
				unsigned int edge = edgeTable[bitmap];
				unsigned int vertNr = numVertsTable[bitmap];
				
				if (vertNr >= 255 || vertNr <= 0)
				{
					continue;
				}
				for (int i = 0; i < vertNr/3; i++)
				{
					int vert  = i*3;
					mVertList[mVertUsed]   = vertsPos[triTable[bitmap][vert]][0] + (float)x;
					mVertList[mVertUsed+1] = vertsPos[triTable[bitmap][vert]][1] + (float)y;
					mVertList[mVertUsed+2] = vertsPos[triTable[bitmap][vert]][2] + (float)z;
					mTriList[mTriUsed] = mVertUsed;					
					mVertUsed += 3;
					mTriUsed += 1;

					vert += 1;
					mVertList[mVertUsed]   = vertsPos[triTable[bitmap][vert]][0] + (float)x;
					mVertList[mVertUsed+1] = vertsPos[triTable[bitmap][vert]][1] + (float)y;
					mVertList[mVertUsed+2] = vertsPos[triTable[bitmap][vert]][2] + (float)z;
					mTriList[mTriUsed] = mVertUsed;
					mVertUsed += 3;
					mTriUsed += 1;

					vert += 1;
					mVertList[mVertUsed]   = vertsPos[triTable[bitmap][vert]][0] + (float)x;
					mVertList[mVertUsed+1] = vertsPos[triTable[bitmap][vert]][1] + (float)y;
					mVertList[mVertUsed+2] = vertsPos[triTable[bitmap][vert]][2] + (float)z;
					mTriList[mTriUsed] = mVertUsed;
					mVertUsed += 3;
					mTriUsed += 1;
				}
			}
		}
	}
	
	//-- Generate normals
	Vector3 vec1, vec2, vec3;
	Vector3 normal;
	for (int i = 0; i < mVertUsed; i+=9)
	{	
		vec1 = Vector3(mVertList[i],mVertList[i+1],mVertList[i+2]);
		vec2 = Vector3(mVertList[i+3],mVertList[i+4],mVertList[i+5]);
		vec3 = Vector3(mVertList[i+6],mVertList[i+7],mVertList[i+8]);
		Vector3 edge1 = vec2 - vec1;
		Vector3 edge2 = vec3 - vec1;
		normal = edge1.Cross(edge2);
		normal.Normalize();

		mNormList[i]   = normal.x;
		mNormList[i+1] = normal.y;
		mNormList[i+2] = normal.z;

		mNormList[i+3] = normal.x;
		mNormList[i+4] = normal.y;
		mNormList[i+5] = normal.z;

		mNormList[i+6] = normal.x;
		mNormList[i+7] = normal.y;
		mNormList[i+8] = normal.z;
	}
}

void CPUTerrain::Render()
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
	glBegin(GL_TRIANGLES);
	{
		for (int i = 0; i < mTriUsed; i++)
		{
			glNormal3f(mNormList[mTriList[i]], mNormList[mTriList[i]+1], mNormList[mTriList[i]+2]);
			glVertex3f(mVertList[mTriList[i]]*0.05f, mVertList[mTriList[i]+1]*0.05f, mVertList[mTriList[i]+2]*0.05f);
		}
	}
	glEnd();

	glPopMatrix();
}