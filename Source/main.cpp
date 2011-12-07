#include <iostream>
#include <string>
#include <cmath>


extern void		Initialize(unsigned int nSeed);
extern float	Noise1(float x);


int main(int argc, char** argv)
{
	Initialize(1);
	float bla = Noise1(10);

	return 0;
}