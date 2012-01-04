#include <windows.h>
#include <WinUser.h>

#include <iostream>
#include <string>
#include <cmath>

#include "Render/Renderer.h"
#include "Render/Camera.h"

Camera* _cam;
void CheckControlls(float aDT)
{
	if (GetAsyncKeyState(VK_UP))
	{
		_cam->moveLoc(0,0,1,1 * aDT);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		_cam->moveLoc(0,0,-1,1 * aDT);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		_cam->moveLoc(1,0,0,1 * aDT);
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		_cam->moveLoc(-1,0,0,1 * aDT);
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
	Renderer* render = Renderer::Create(argc, argv);
	_cam = render->GetCam();

	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	float elapsedTime = 0.2;
	QueryPerformanceFrequency(&frequency);


	while (true)
	{
		
		QueryPerformanceCounter(&t1);

		CheckControlls(elapsedTime);	
		render->Update(elapsedTime);	

		QueryPerformanceCounter(&t2);
		elapsedTime = (float)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;
	}
	



	return 0;
}