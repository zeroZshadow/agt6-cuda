#pragma once;

#include "..\Misc\Effect.h"

class CPUMarcher;
class CUDAMarcher;
class Camera;

class Renderer
{
public:
	static Renderer*	Create(int argc, char* argv[]);
	static void			Destory();

	void				Update(float aDT);
	void				Render();
	void				Resize(int w, int h);
	
	Camera*				GetCam(){return mCam;}				

protected:
	Renderer(int argc, char* argv[]);
	~Renderer();

	void				_InitOpenGL(int argc, char* argv[]);
	void				_InitCg();
	void				_InitWindow(unsigned int x, unsigned int y);
	void				_InitLighting();
	void				_Resize();

	CPUMarcher*			mCPUMarcher;
	CUDAMarcher*		mCUDAMarcher;

	Effect*				m_Shader;
	CGeffect			m_Effect;
	CGtechnique			m_Technique;
	CGpass				m_Pass;

	CGparameter			m_Param_EyePosition;

	Camera*				mCam;
	
};