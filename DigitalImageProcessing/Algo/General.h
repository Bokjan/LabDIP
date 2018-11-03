#pragma once
#include "../stdafx.h"

struct ParallelParams
{
	CImage *img;
	int wParam;
	int begin, end;
	void *ctx, *thctx;
	void(*cb)(ParallelParams*);
	ParallelParams(void) :
		wParam(1),
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
	UINT FourierTransform(LPVOID params);
	UINT FastFourierTransform(LPVOID params);
	UINT GaussianNoise(LPVOID params);
	UINT ArithMeanFilter(LPVOID params);
	UINT GaussianFilter(LPVOID params);
	UINT WienerFilter(LPVOID params);

	UINT ImageScaleCL(LPVOID params);
	UINT ImageRotateCL(LPVOID params);
	UINT FourierTransformCL(LPVOID params);

	ParallelParams* SplitTask(CImage *img, int thread);
	size_t RoundUp(size_t group, size_t global);

	struct ScaleParams
	{
		CImage *src;
		double scale;
	};

	struct RotateParams
	{
		CImage *src;
		double angle;
	};

	struct GaussianParams
	{
		CImage *src;
		double mean, stddev;
	};
	
	template <typename T>
	struct vec2
	{
		T x, y;
	};
}

class CImageWrapper
{
private:
	CImage *img;
public:
	byte *mem;
	bool IsGray;
	int Width, Height, Pitch, BytePP;
	inline CImageWrapper(CImage *img) :
		img(img), Pitch(img->GetPitch()),
		Width(img->GetWidth()),	Height(img->GetHeight()), 
		BytePP(img->GetBPP() / 8), IsGray(BytePP == 1),
		mem((byte*)img->GetBits()) { }
	inline byte* MemStartAt(void)
	{
		return mem + Pitch * (Height - 1);
	}
	inline size_t MemSize(void)
	{
		return -Pitch * Height * sizeof(byte);
	}
	inline void SetPixel(int x, int y, const byte value)
	{
		*(mem + Pitch * y + BytePP * x) = value;
	}
	inline void SetPixel(int x, int y, const byte r, const byte g, const byte b)
	{
		*(mem + Pitch * y + BytePP * x + 0) = r;
		*(mem + Pitch * y + BytePP * x + 1) = g;
		*(mem + Pitch * y + BytePP * x + 2) = b;
	}
	inline void SetPixel(int x, int y, const byte *ptr)
	{
		*(mem + Pitch * y + BytePP * x + 0) = ptr[0];
		*(mem + Pitch * y + BytePP * x + 1) = ptr[1];
		*(mem + Pitch * y + BytePP * x + 2) = ptr[2];
	}
	inline byte* GetPixel(int x, int y)
	{
		return (byte*)(mem + Pitch * y + BytePP * x);
	}
};