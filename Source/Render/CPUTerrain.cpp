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
	mWidth = width;
	mHeight = height;
	mDepth = depth;
	mData = new float[width * height * depth];

	float piDev = PI * 5;

	for (int x = 0; x < mWidth; x++)
	{
		for (int y = 0; y < mHeight; y++)
		{
			for (int z = 0; z < mDepth; z++)
			{
				mData[ (x * (int)mHeight * (int)mDepth) + (y * (int)mDepth) + z ] = mPerlin->Noise3((float)x / piDev,(float)y / piDev,(float)z / piDev);
			}
		}
	}
}

void CPUTerrain::Render()
{
	glPointSize(15);
	glColor3f(1,1,1);

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

}