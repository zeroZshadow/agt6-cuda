#include <cutil_inline.h>

__global__ void cuda_CreateCube(float3* aVertList, float3* aNormList, unsigned int* aIndexList)
{
	//aVertList, aNormList and aIndexList are simple arrays with the same size as the VBO's
	//You can fill them like normal and they will be rendered propperly
}

extern "C"
void launch_CreateCube(dim3 grid, dim3 threads, float3* aVertList, float3* aNormList, unsigned int* aIndexList)
{
	//Here you normally run:
	cuda_CreateCube<<<grid, threads>>>(aVertList, aNormList, aIndexList);
	cutilCheckMsg("cuda_CreateCube failed");
	
	//TEST
	
	aVertList[0] = make_float3(1,0,0);
	aVertList[1] = make_float3(0,1,0);
	aVertList[2] = make_float3(0,0,0);
	
	aNormList[0] = make_float3(0,0,1);
	aNormList[1] = make_float3(0,0,1);
	aNormList[2] = make_float3(0,0,1);
	
	aIndexList[0] = 0;
	aIndexList[1] = 1;
	aIndexList[2] = 2;
}