//-- Needed a quick cam that dosnt use a external math lib like mine, so 
//-- i took code taken from http://www.flipcode.com/archives/OpenGL_Camera.shtml
#include "Camera.h"
#include "windows.h"
#include <gl\gl.h>			// Header File For The OpenGL32 Library


/*
The constructor will set the matrix to identity, except the inverted z-axis.
I cant get used to "forward" being negative, but if this bothers you feel free
to change the marked parts.

Also, if you dont like to access the vectors like float-arrays you can always
use vector-pointers, as long as they only contain three floats like this:
struct vector{float x,y,z;};
and replace the float* with vector* in the header file.
*/
Camera::Camera(float x, float y, float z) {
	memset(Transform, 0, 16*sizeof(float));
	Transform[0] = 1.0f;
	Transform[5] = 1.0f;
	Transform[10] = -1.0f;
	Transform[15] = 1.0f;
	Transform[12] = x; Transform[13] = y; Transform[14] = z;

	Right=&Transform[0];
	Up=&Transform[4];
	Forward=&Transform[8];
	Position=&Transform[12];
}

Camera::~Camera() {}

/*
This one does pretty much the same as gluLookAt, just that it doesnt require
to extract the vectors for gluLookAt and have it rebuild the matrix we already
got.
*/
void Camera::setView() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	float viewmatrix[16]={//Remove the three - for non-inverted z-axis
						  Transform[0], Transform[4], -Transform[8], 0,
						  Transform[1], Transform[5], -Transform[9], 0,
						  Transform[2], Transform[6], -Transform[10], 0,

						  -(Transform[0]*Transform[12] +
						  Transform[1]*Transform[13] +
						  Transform[2]*Transform[14]),

						  -(Transform[4]*Transform[12] +
						  Transform[5]*Transform[13] +
						  Transform[6]*Transform[14]),

						  //add a - like above for non-inverted z-axis
						  (Transform[8]*Transform[12] +
						  Transform[9]*Transform[13] +
						  Transform[10]*Transform[14]), 1};
	glLoadMatrixf(viewmatrix);
}

void Camera::setLoc(float x, float y, float z)
{
	memset(Transform, 0, 16*sizeof(float));
	Transform[0] = 1.0f;
	Transform[5] = 1.0f;
	Transform[10] = -1.0f;
	Transform[15] = 1.0f;
	Transform[12] = x; Transform[13] = y; Transform[14] = z;

	Right=&Transform[0];
	Up=&Transform[4];
	Forward=&Transform[8];
	Position=&Transform[12];
}

void Camera::moveLoc(float x, float y, float z, float distance) {
	float dx=x*Transform[0] + y*Transform[4] + z*Transform[8];
	float dy=x*Transform[1] + y*Transform[5] + z*Transform[9];
	float dz=x*Transform[2] + y*Transform[6] + z*Transform[10];
	Transform[12] += dx * distance;
	Transform[13] += dy * distance;
	Transform[14] += dz * distance;
}

void Camera::moveGlob(float x, float y, float z, float distance) {
	Transform[12] += x * distance;
	Transform[13] += y * distance;
	Transform[14] += z * distance;
}

/*
Here we let OpenGls (most likely quite optimized) functions do the work.
Note that its transformations already are in local coords.
*/
void Camera::rotateLoc(float deg, float x, float y, float z) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(Transform);
	glRotatef(deg, x,y,z);
	glGetFloatv(GL_MODELVIEW_MATRIX, Transform);
	glPopMatrix();
}

/*
We have to invert the rotations to get the global axes in local coords.
Luckily thats just the transposed in this case.
*/
void Camera::rotateGlob(float deg, float x, float y, float z) {
	float dx=x*Transform[0] + y*Transform[1] + z*Transform[2];
	float dy=x*Transform[4] + y*Transform[5] + z*Transform[6];
	float dz=x*Transform[8] + y*Transform[9] + z*Transform[10];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(Transform);
	glRotatef(deg, dx,dy,dz);
	glGetFloatv(GL_MODELVIEW_MATRIX, Transform);
	glPopMatrix();
}