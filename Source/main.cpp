#include <windows.h>
#include <WinUser.h>

#include <iostream>
#include <string>
#include <cmath>

#include "Render/Renderer.h"
#include "Render/Camera.h"

#include <cutil_inline.h>    // includes cuda.h and cuda_runtime_api.h

//-- GUI
#include "../GUI.h"
#include "Render/Defines.h"

Camera* _cam;
Renderer* _render;
void CheckControlls(float aDT)
{
	if (GetAsyncKeyState(VK_UP))
	{
		_cam->moveLoc(0,0,1,10 * aDT);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		_cam->moveLoc(0,0,-1,10 * aDT);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		_cam->moveLoc(1,0,0,10 * aDT);
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		_cam->moveLoc(-1,0,0,10 * aDT);
	}

	if (GetAsyncKeyState('A'))
	{
		_cam->rotateLoc(50 * aDT, 0,-1,0);
	}
	if (GetAsyncKeyState('D'))
	{
		_cam->rotateLoc(50 * aDT, 0,1,0);
	}
	if (GetAsyncKeyState('W'))
	{
		_cam->rotateLoc(50 * aDT, -1,0,0);
	}
	if (GetAsyncKeyState('S'))
	{
		_cam->rotateLoc(50 * aDT, 1,0,0);
	}
}


int main(int argc, char** argv)
{
	_render = Renderer::Create(argc, argv);
	_cam = _render->GetCam();
	Application::Run(gcnew CudaTest::GUI());

	cutilDeviceReset();
	return 0;
}

void updateProgram()
{
	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	float elapsedTime = 0.2;
	QueryPerformanceFrequency(&frequency);

	QueryPerformanceCounter(&t1);

	
	_render->Update(elapsedTime);	
	

	QueryPerformanceCounter(&t2);
	elapsedTime = (float)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;
	CheckControlls(elapsedTime);	
}

void generateTerrain(GenerateInfo agInfo)
{
	_render->GenerateTerrain(agInfo);
}