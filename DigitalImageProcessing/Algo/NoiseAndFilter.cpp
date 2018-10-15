#include "General.h"
#include "../Util/DisplayAgent.h"
#define NOISE_FACTOR (0.1)

UINT Algo::SaltAndPepperNoise(LPVOID _params)
{
	srand(static_cast<unsigned int>(time(nullptr)));
	ParallelParams *params = (ParallelParams*)_params;
	int begin = params->begin;
	int end = params->end;
	CImageWrapper img(params->img);
	for (int i = begin; i < end; ++i)
	{
		int x = i % img.Width;
		int y = i / img.Width;
		if ((rand() / (double)RAND_MAX) <= NOISE_FACTOR)
		{
			byte val = (rand() & 0x1) ? 0 : 255;
			if (img.IsGray) // »Ò¶È
				img.SetPixel(x, y, val);
			else
				img.SetPixel(x, y, CImageWrapper::Color(val, val, val));
		}
	}
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}

UINT Algo::MedianFilter(LPVOID params)
{
	return 0;
}