//-- OpenGL implementation headers
#include "windows.h"
#include "glew.h"
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "glut.h"
#include "freeglut_ext.h"
//-- GL End

//-- Misc
#include <iostream>
//-- Misc End

//-- Render headers
#include "Renderer.h"
#include "CPUMarcher.h"
#include "CUDAMarcher.h"
#include "Camera.h"
#include "..\Misc\Vector3.h"
#include "..\Misc\CGManager.h"
#include <Cg/cgGL.h>
//-- Render End

#include <cutil_inline.h>
#include <cuda_gl_interop.h>

static Renderer* _RenderImpl = 0;

//-- Callbacks:
void GLResize(GLsizei w, GLsizei h){_RenderImpl->Resize(w,h); }
//void GLSetupRC(void){ glClearColor(0,0,0,0)}
void GLRenderCallback(){_RenderImpl->Render();}
//-- End callbacks



Renderer* Renderer::Create(int argc, char* argv[])
{
	if(_RenderImpl == NULL)
	{
		_RenderImpl = new Renderer(argc, argv);
	}
	return _RenderImpl;
}

void Renderer::Destory()
{
	if (_RenderImpl != NULL)
	{
		delete _RenderImpl;
	}
}

Renderer::Renderer(int argc, char* argv[])
{
	_InitOpenGL(argc, argv);

	//Setup Cg
	_InitCg();

	//Setup Marchers
	mCPUMarcher = new CPUMarcher();
	mCPUMarcher->Init(32,32,32);
	mCPUMarcher->Cubemarch();

	mCUDAMarcher = new CUDAMarcher();
	mCUDAMarcher->Init(32, 1); //Rank, Blockcount
	mCUDAMarcher->Cubemarch();


	mCam = new Camera();
}

Renderer::~Renderer()
{
	//-- Empty
	delete mCPUMarcher;
	delete mCUDAMarcher;
	delete mCam;
}

void Renderer::Render()
{
	//-- Clear the window with current clearing color
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0f);
	glMatrixMode( GL_MODELVIEW );                                           // Switch to modelview matrix mode
	//-- glLoadIdentity();      

	mCam->setView();

	float pos[4] = {0,0,1,0};
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	cgGLSetParameter3fv(m_Param_EyePosition, mCam->Position);

	//Cg time!
	cgSetPassState(m_Pass);
		//mCPUMarcher->Render();
		mCUDAMarcher->Render();
	cgResetPassState(m_Pass);

	glutSwapBuffers();
	glutPostRedisplay();

}

void Renderer::Update(float aDT)
{
	glutMainLoopEvent();
}

void Renderer::_InitOpenGL(int argc, char* argv[])
{
	//Setup Opengl
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH);

	_InitWindow(800, 600);

	if ( glewInit() != GLEW_OK )
	{
		std::cout << "Failed to initilalize GLEW!" << std::endl;
		exit(-1);
	}

	//Setup CUDA
	cutilChooseCudaDevice(argc, argv);
	cudaGLSetGLDevice( cutGetMaxGflopsDeviceId() );


	//Opengl options
	glutDisplayFunc(GLRenderCallback);
	glClearColor(0,1,0,0);
	glShadeModel( GL_FLAT );
	glEnable( GL_TEXTURE_2D );

	glEnable( GL_DEPTH_TEST );
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_NORMALIZE);

	_InitLighting();
}

void Renderer::_InitWindow(unsigned int x, unsigned int y)
{
	glutInitWindowSize(x,y);
	glutCreateWindow("Martijn Gerkers - Chris Hekman");
	glutReshapeFunc(GLResize);
}

void Renderer::_InitLighting()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat g_LighDir[] = { 1.0f, 0.0f, 0.0f, 0.0f }; 
	GLfloat g_LightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat g_LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat g_LightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat g_LighAttenuation0 = 1.0f;
	GLfloat g_LighAttenuation1 = 0.0f;
	GLfloat g_LighAttenuation2 = 0.0f;

	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, g_LightAmbient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, g_LightDiffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, g_LightSpecular );
	glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, g_LighAttenuation0 );
	glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, g_LighAttenuation1 );
	glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, g_LighAttenuation2 );
}

void Renderer::_InitCg()
{
	CGManager::GetInstance()->Initialize();
	m_Shader = CGManager::GetInstance()->LoadEffect("Assets/Shaders/TextureProjection.cgfx", "TextureProjection");
	
	m_Param_EyePosition  = m_Shader->GetParameter("gEyePosition");

	m_Effect = m_Shader->GetEffect();
	m_Technique = cgGetFirstTechnique(m_Effect);
	if (!cgValidateTechnique(m_Technique)) {
		printf("TECH IS BROKEN!\n");
	}
	m_Pass = cgGetFirstPass(m_Technique);
}

void Renderer::Resize(int w, int h)
{
	GLfloat nRange = 100.0f;
	// Prevent a divide by zero
	if(h == 0)
		h = 1;
	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);
	// Reset projection matrix stack
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 60.0, (GLdouble)w/(GLdouble)h, 0.1, 1000.0 );
}