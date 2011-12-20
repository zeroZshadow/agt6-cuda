#pragma once;


class CUDATerrain
{
public:
	CUDATerrain();
	~CUDATerrain();

	void		Build(int width, int height, int depth);
	void		Destroy();

	void		Render();

protected:
	float		mWidth, mHeight, mDepth;

};