#ifndef PERLIN_H
#define PERLIN_H

//#define STRICT				// Enable strict type checking

#define SIZE 256
#define MASK 0xFF

class CPerlin
{
public:
	CPerlin();
		
	void Initialize(unsigned int nSeed);

	float Noise1(float x);
	float Noise2(float x, float y);
	float Noise3(float x, float y, float z);

protected:
	// Permutation table
	unsigned char p[SIZE];

	// Gradients
	float gx[SIZE];
	float gy[SIZE];
	float gz[SIZE];
};

#endif