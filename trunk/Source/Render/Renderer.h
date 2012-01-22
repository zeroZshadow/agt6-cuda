#pragma once;

#include "..\Misc\Effect.h"
#include "Defines.h"

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

	void				UpdateGenInfo(GenerateInfo agInfo);
	void				GenerateTerrain(GenerateInfo agInfo);
	void				DrawCubemap();

protected:
	Renderer(int argc, char* argv[]);
	~Renderer();

	void				_InitOpenGL(int argc, char* argv[]);
	void				_InitCg();
	void				_InitWindow(unsigned int x, unsigned int y);
	void				_InitLighting();
	void				_Resize();
	void				_LoadTextures();

	CPUMarcher*			mCPUMarcher;
	CUDAMarcher*		mCUDAMarcher;

	Effect*				m_Shader;
	CGeffect			m_Effect;
	CGtechnique			m_Technique;
	CGpass				m_Pass;

	unsigned int		m_Textures[3];
	unsigned int		m_SkyTexture;

	CGparameter			m_Param_EyePosition;
	CGparameter			m_Param_ProjSampler[3];

	Camera*				mCam;
	GenerateInfo		mGenInfo;
	
};