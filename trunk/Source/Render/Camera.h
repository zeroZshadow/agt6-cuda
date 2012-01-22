//-- Needed a quick cam that dosnt use a external math lib like mine own, so 
//-- i took code from http://www.flipcode.com/archives/OpenGL_Camera.shtml
#pragma once;

class Camera {
public:
	float const *Right, *Up, *Forward;
	float *Position;
private:
	float Transform[16];

public:
	Camera(float x=0.0f, float y=0.0f, float z=0.0f);
	~Camera();

	void setView();
	void setLoc(float x, float y, float z);
	void moveLoc(float x, float y, float z, float distance=1);
	void moveGlob(float x, float y, float z, float distance=1);
	void rotateLoc(float deg, float x, float y, float z);
	void rotateGlob(float deg, float x, float y, float z);
};

