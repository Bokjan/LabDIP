#pragma once
#include "../../stdafx.h"

struct AfxParams
{
	CImage *img;
	int maxspan;
	int begin, end;
};

namespace AlgoAfx
{
	UINT SaltAndPepperNoise(LPVOID params);
	UINT MedianFilter(LPVOID params);
}