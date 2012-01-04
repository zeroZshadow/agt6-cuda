#pragma once;

class CPerlin;

class Vector3;
class Triangle;

struct MarchBlock
{
	MarchBlock()
	{
		VertList = 0;
		NormList = 0;
		VertFilled = 0;
		TriList = 0;
		TriFilled = 0;
	}
	Vector3*	VertList;
	Vector3*	NormList;
	int			VertFilled;
	Triangle*	TriList;
	int			TriFilled;
};

class CPUMarcher
{
public:
	CPUMarcher();
	~CPUMarcher();
	void		Init(int width, int height, int depth);

	void		FillSphere();
	void		FillPerlin();

	void		Cubemarch();
	
	void		Destroy();
	void		Render();


protected:
	void		_CalculateNormals(MarchBlock* aBlock);

	CPerlin*	mPerlin;
	float*		mData;

	float		mWidth, mHeight, mDepth;
	
	int			mVertCount;
	int			mTriCount;
	int			mNormCount;

	//--
	MarchBlock*	mBlocks;
	int			mNrBlocks;

};