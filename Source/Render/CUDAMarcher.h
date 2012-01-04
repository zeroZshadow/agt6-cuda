#pragma once;


class CUDAMarcher
{
public:
	CUDAMarcher();
	~CUDAMarcher();

	void		Build(int width, int height, int depth);
	void		Destroy();

	void		Render();

protected:
	float		mWidth, mHeight, mDepth;


};