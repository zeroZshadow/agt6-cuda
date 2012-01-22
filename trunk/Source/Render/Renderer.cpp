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

#include "TinyImageLoader.h"

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

void Renderer::GenerateTerrain(GenerateInfo agInfo)
{
	mCUDAMarcher->GenerateTerrain(agInfo);
}

Renderer::Renderer(int argc, char* argv[])
{
	_InitOpenGL(argc, argv);

	//Setup Cg
	_InitCg();

	//Load Textures
	_LoadTextures();

	//Setup Marchers
	//mCPUMarcher = new CPUMarcher();
	//mCPUMarcher->Init(32,32,32);
	//mCPUMarcher->Cubemarch();

	mCUDAMarcher = new CUDAMarcher();
	//mCUDAMarcher->GenerateTerrain(GenerateInfo());

	mCam = new Camera(1.6f, 7.f, 5.f);
	mCam->rotateLoc(40, 1,0,0);
}

Renderer::~Renderer()
{
	//-- Empty
	delete mCPUMarcher;
	delete mCUDAMarcher;
	delete mCam;
	til::TIL_ShutDown();
}

void Renderer::Render()
{
	//-- Clear the window with current clearing color
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0f);
	glMatrixMode( GL_MODELVIEW );                                           // Switch to modelview matrix mode
	//-- glLoadIdentity();      

	mCam->setView();

	DrawCubemap();

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

	//Setup TIL
	til::TIL_Init();

	//Setup CUDA
	cutilChooseCudaDevice(argc, argv);
	cudaGLSetGLDevice( cutGetMaxGflopsDeviceId() );


	//Opengl options
	glutDisplayFunc(GLRenderCallback);
	glClearColor(0.2f,0.2f,1.f,0);
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
	glutCreateWindow("Martijn Gerkes - Chris Hekman");
	glutReshapeFunc(GLResize);
}

void Renderer::_InitLighting()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat g_LighDir[] = { 0.0f, -0.5f, 0.5f, 0.0f }; 
	GLfloat g_LightAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat g_LightDiffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat g_LightSpecular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat g_LighAttenuation0 = 1.0f;
	GLfloat g_LighAttenuation1 = 0.0f;
	GLfloat g_LighAttenuation2 = 0.0f;

	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, g_LightAmbient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, g_LightDiffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, g_LightSpecular );
	glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, g_LighAttenuation0 );
	glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, g_LighAttenuation1 );
	glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, g_LighAttenuation2 );

	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.5f );
}

void Renderer::_InitCg()
{
	CGManager::GetInstance()->Initialize();
	m_Shader = CGManager::GetInstance()->LoadEffect("./Assets/Shaders/TextureProjection.cgfx", "TextureProjection");
	
	m_Param_EyePosition  = m_Shader->GetParameter("gEyePosition");
	m_Param_ProjSampler[0] = m_Shader->GetParameter("ProjSampler1");
	m_Param_ProjSampler[1] = m_Shader->GetParameter("ProjSampler2");
	m_Param_ProjSampler[2] = m_Shader->GetParameter("ProjSampler3");

	m_Effect = m_Shader->GetEffect();
	m_Technique = cgGetFirstTechnique(m_Effect);
	if (!cgValidateTechnique(m_Technique)) {
		printf("TECH IS BROKEN!\n");
	}
	m_Pass = cgGetFirstPass(m_Technique);
}

void Renderer::_LoadTextures()
{
	//Load terrain textures
	til::Image* image[3];
	image[0] = til::TIL_Load("./Assets/Textures/RoughSoil.png", TIL_FILE_ABSOLUTEPATH | TIL_DEPTH_A8B8G8R8);	//Side
	image[1] = til::TIL_Load("./Assets/Textures/LeavesDead.png", TIL_FILE_ABSOLUTEPATH | TIL_DEPTH_A8B8G8R8);		//Top
	image[2] = til::TIL_Load("./Assets/Textures/SoilTop.png", TIL_FILE_ABSOLUTEPATH | TIL_DEPTH_A8B8G8R8);	//Side

	glGenTextures(3, m_Textures);

	for (unsigned int i = 0; i < 3; i++)
	{
		if(image[i]==0)
		{
			printf("> Could not load texture\n");
			continue;
		}
		glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA,
			image[i]->GetPitchX(), image[i]->GetPitchY(),
			0,
			GL_RGBA, GL_UNSIGNED_BYTE, image[i]->GetPixels()
		);
		glBindTexture(GL_TEXTURE_2D, 0);

		cgGLSetupSampler(m_Param_ProjSampler[i], m_Textures[i]);

		til::TIL_Release(image[i]);
	}

	//Load Sky texture
	til::Image* skytex;
	skytex = til::TIL_Load("./Assets/Textures/Sky.tga", TIL_FILE_ABSOLUTEPATH | TIL_DEPTH_A8B8G8R8);	//Side

	glGenTextures(1,&m_SkyTexture);
	glEnable( GL_TEXTURE_CUBE_MAP );
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_SkyTexture );

	for (int face=0; face<6; face++) {
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X+face, 0, GL_RGBA,
			skytex->GetPitchX(), skytex->GetPitchY(),
			0,
			GL_RGBA, GL_UNSIGNED_BYTE, skytex->GetPixels()
			);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	glDisable( GL_TEXTURE_CUBE_MAP );

	til::TIL_Release(skytex);

}

void Renderer::DrawCubemap()
{
	glPushMatrix();

	//glLoadIdentity();
	glTranslatef(mCam->Position[0], mCam->Position[1], mCam->Position[2]);

	glDisable(GL_CULL_FACE);
	glDepthMask( GL_FALSE );  // Don't write to the depth buffer
	glEnable( GL_TEXTURE_CUBE_MAP );
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_SkyTexture );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ); // Don't do any blending on the cube map textures

	glBegin( GL_QUADS );
	{
		glTexCoord3f(  1, -1, -1 );
		glVertex3f(    1, -1, -1 );
		glTexCoord3f(  1,  1, -1 );
		glVertex3f(    1,  1, -1 );
		glTexCoord3f(  1,  1,  1 );
		glVertex3f(    1,  1,  1 );
		glTexCoord3f(  1, -1,  1 );
		glVertex3f(    1, -1,  1 );
		glTexCoord3f( -1, -1, -1 );
		glVertex3f(   -1, -1, -1 );
		glTexCoord3f( -1,  1, -1 );
		glVertex3f(   -1,  1, -1 );
		glTexCoord3f( -1,  1,  1 );
		glVertex3f(   -1,  1,  1 );
		glTexCoord3f( -1, -1,  1 );
		glVertex3f(   -1, -1,  1 );
		glTexCoord3f( -1,  1, -1 );
		glVertex3f(   -1,  1, -1 );
		glTexCoord3f(  1,  1, -1 );
		glVertex3f(    1,  1, -1 );
		glTexCoord3f(  1,  1,  1 );
		glVertex3f(    1,  1,  1 );
		glTexCoord3f( -1, 1,  1 );
		glVertex3f(   -1, 1,  1 );
		glTexCoord3f( -1, -1, -1 );
		glVertex3f(   -1, -1, -1 );
		glTexCoord3f(  1, -1, -1 );
		glVertex3f(    1, -1, -1 );
		glTexCoord3f(  1, -1,  1 );
		glVertex3f(    1, -1,  1 );
		glTexCoord3f( -1, -1,  1 );
		glVertex3f(   -1, -1,  1 );
		glTexCoord3f( -1, -1,  1 );
		glVertex3f(   -1, -1,  1 );
		glTexCoord3f(  1, -1,  1 );
		glVertex3f(    1, -1,  1 );
		glTexCoord3f(  1,  1,  1 );
		glVertex3f(    1,  1,  1 );
		glTexCoord3f( -1,  1,  1 );
		glVertex3f(   -1,  1,  1 );
		glTexCoord3f( -1, -1, -1 );
		glVertex3f(   -1, -1, -1 );
		glTexCoord3f(  1, -1, -1 );
		glVertex3f(    1, -1, -1 );
		glTexCoord3f(  1,  1, -1 );
		glVertex3f(    1,  1, -1 );
		glTexCoord3f( -1,  1, -1 );
		glVertex3f(   -1,  1, -1 );
		glEnd();
	}

	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	glDisable( GL_TEXTURE_CUBE_MAP );
	glDepthMask( GL_TRUE );
	glEnable(GL_CULL_FACE);

	glPopMatrix();
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