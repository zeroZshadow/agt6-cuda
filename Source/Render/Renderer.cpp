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
#include "CPUTerrain.h"
#include "Camera.h"
//-- Render End

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
	mCPUTerrain = new CPUTerrain();
	mCPUTerrain->Build(20,20,20);

	mCam = new Camera();
}

Renderer::~Renderer()
{
	//-- Empty
	delete mCPUTerrain;
	delete mCam;
}

void Renderer::Render()
{
	// Clear the window with current clearing color
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0f);
	glMatrixMode( GL_MODELVIEW );                                           // Switch to modelview matrix mode
	glLoadIdentity();      

	mCam->setView();
	
	mCPUTerrain->Render();

	glutSwapBuffers();
	glutPostRedisplay();

}

void Renderer::Update(float aDT)
{
	glutMainLoopEvent();
}

void Renderer::_InitOpenGL(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH);

	_InitWindow(800, 600);

	if ( glewInit() != GLEW_OK )
	{
		std::cout << "Failed to initilalize GLEW!" << std::endl;
		exit(-1);
	}

	glutDisplayFunc(GLRenderCallback);
	glClearColor(0,0,0,0);
	glShadeModel( GL_SMOOTH );
	glEnable( GL_TEXTURE_2D );

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