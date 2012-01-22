#ifndef DEFINES_H
#define DEFINES_H

#define PERLIN_DATA_RANK 33
#define PERLIN_DATA_RANK2 43 //prime number so it repeats asyncronous
#define PERLIN_DATA_RANK3 53

#define MARCHING_BLOCK_SIZE 32
#define MARCHING_BLOCK_SIZE_POWER2 1024
#define MARCHING_BLOCK_SIZE_POWER3 32768

enum GenerationMethod
{
	GM_FLOOR,
	GM_CAVES,
	GM_SPHERE
};

enum SampleMethod
{
	SM_PREMADE,
	SM_PERLIN
};

struct GenerateInfo
{
	GenerateInfo()
	{
		genType = GM_FLOOR;
		sampleType = SM_PREMADE;
		sphereRad = 10;
		floor = 0.1;

		prlnWeight1 = 1;
		prlnWeight2 = 0.75;
		prlnWeight3 = 0.5;
		prlnWeight4 = 0.25;
		prlnNoise1 = 5.0f;
		prlnNoise2 = 1.0f;
		prlnNoise3 = 0.5f;
		prlnNoise4 = 0.25f;
		gridRank = 2;
		iso = 0.5;
	}
	GenerationMethod genType;
	SampleMethod sampleType;
	float sphereRad;
	float floor;
	
	float prlnWeight1;
	float prlnWeight2;
	float prlnWeight3;
	float prlnWeight4;
	float prlnNoise1;
	float prlnNoise2;
	float prlnNoise3;
	float prlnNoise4;
	int		gridRank;
	float	iso;
};

#define PERLIN_WEIGHT_RANGE_MIN 0.f
#define PERLIN_WEIGHT_RANGE_MAX 100.f

#define PERLIN_NOISE_RANGE_MIN 0.001f
#define PERLIN_NOISE_RANGE_MAX 50.f

#define SPHERE_GEN_RANGE_MIN 1.0f
#define SPHERE_GEN_RANGE_MAX 80.0f

#define FLOOR_GEN_RANGE_MIN 0.01f
#define FLOOR_GEN_RANGE_MAX 100.0f

#define QUICK_RANGE_MAP(x, minA, maxA, minB, maxB) (    ((x-minA) * ((maxB - minB) / (maxA - minA)) + minB)    )


#endif	DEFINES_H
#define PERLIN_DATA_RANK 33
#define PERLIN_DATA_SIZE 34
#define MARCHING_BLOCK_SIZE 32