#pragma once;

class CPerlin;

class CPUTerrain
{
public:
	CPUTerrain();
	~CPUTerrain();

	void		Build(int width, int height, int depth);
	void		Destroy();
	void		Render();

protected:
	CPerlin*	mPerlin;
	float*		mData;

	float		mWidth, mHeight, mDepth;
	
	float*		mVertList;
	int			mVertCount;
	int			mVertUsed;
	int*		mTriList;
	int			mTriCount;
	int			mTriUsed;
	float*		mNormList;
	int			mNormCount;
	int			mNormUsed;


};