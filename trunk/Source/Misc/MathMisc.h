//Code from Jacco bikker's tmpl8
#pragma once
#include "math.h"
//#include "stdlib.h"

//--Math macros
//inline float Rand( float a_Range ) { return ((float)rand() / RAND_MAX) * a_Range; }

#define MIN(a,b) (((a)>(b))?(b):(a))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define _fabs	fabsf
#define _cos	cosf
#define _sin	sinf
#define _acos	acosf
#define _floor	floorf
#define _ceil	ceilf
#define _sqrt	sqrtf
#define _pow	powf
#define _exp	expf

#define CROSS(A,B)		vector3(A.y*B.z-A.z*B.y,A.z*B.x-A.x*B.z,A.x*B.y-A.y*B.x)
#define DOT(A,B)		(A.x*B.x+A.y*B.y+A.z*B.z)
#define NORMALIZE(A)	{float l=1/_sqrt(A.x*A.x+A.y*A.y+A.z*A.z);A.x*=l;A.y*=l;A.z*=l;}
#define CNORMALIZE(A)	{float l=1/_sqrt(A.r*A.r+A.g*A.g+A.b*A.b);A.r*=l;A.g*=l;A.b*=l;}
#define LENGTH(A)		(_sqrt(A.x*A.x+A.y*A.y+A.z*A.z))
#define SQRLENGTH(A)	(A.x*A.x+A.y*A.y+A.z*A.z)
#define SQRDISTANCE(A,B) ((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y)+(A.z-B.z)*(A.z-B.z))

#define PI				3.14159265358979323846264338327950288419716939937510582097494459072381640628620899862803482534211706798f


