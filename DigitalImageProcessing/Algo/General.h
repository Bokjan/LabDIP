#pragma once
#include "../stdafx.h"

struct ParallelParams
{
	void *ctx;
	CImage *img;
	int begin, end;
};

namespace Algo
{
	UINT SaltAndPepperNoise(LPVOID params);
	UINT MedianFilter(LPVOID params);
}