#pragma once
#include "../stdafx.h"

struct ParallelParams
{
	CImage *img;
	int begin, end;
	void *ctx, *thctx;
	void(*cb)(ParallelParams*);
	ParallelParams(void) :
		cb(nullptr),
		img(nullptr), 
		ctx(nullptr), 
		thctx(nullptr) { }
};

namespace Algo
{
	UINT SaltAndPepperNoise(LPVOID params);
	UINT MedianFilter(LPVOID params);
	UINT ImageScale(LPVOID params);
	UINT ImageRotate(LPVOID params);
	ParallelParams* SplitTask(CImage *img, int thread);

	struct ScaleParams
	{
		CImage *src;
		double scale;
	};
}

class CImageWrapper
{
private:
	CImage *img;
	byte *mem;
public:
	bool IsGray;
	int Width, Height, Pitch, BytePP;
	inline CImageWrapper(CImage *img) :
		img(img), Pitch(img->GetPitch()),
		Width(img->GetWidth()),	Height(img->GetHeight()), 
		BytePP(img->GetBPP() / 8), IsGray(BytePP == 1),
		mem((byte*)img->GetBits()) { }
	inline void SetPixel(int x, int y, const byte value)
	{
		*(mem + Pitch * y + BytePP * x) = value;
	}
	inline void SetPixel(int x, int y, const byte r, const byte g, const byte b)
	{
		*(mem + Pitch * y + BytePP * x) = r;
		*(mem + Pitch * y + BytePP * x + 1) = g;
		*(mem + Pitch * y + BytePP * x + 2) = b;
	}
	inline byte* GetPixel(int x, int y)
	{
		return (byte*)(mem + Pitch * y + BytePP * x);
	}
};