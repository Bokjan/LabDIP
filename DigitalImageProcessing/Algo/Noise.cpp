#include "General.h"
#include "../Util/DisplayAgent.h"

UINT Algo::SaltAndPepperNoise(LPVOID _params)
{
	srand(static_cast<unsigned int>(time(nullptr)));
	auto params = (ParallelParams*)_params;
	CImageWrapper img(params->img);
	for (int i = params->begin; i < params->end; ++i)
	{
		int x = i % img.Width;
		int y = i / img.Width;
#define NOISE_FACTOR (0.1)
		if ((rand() / (double)RAND_MAX) <= NOISE_FACTOR)
		{
			byte val = (rand() & 0x1) ? 0 : 255;
			if (img.IsGray) // »Ò¶È
				img.SetPixel(x, y, val);
			else
				img.SetPixel(x, y, val, val, val);
		}
#undef NOISE_FACTOR
	}
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}

UINT Algo::GaussNoise(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	auto gp = (GaussParams*)(params->ctx);
	CImageWrapper src(gp->src), img(params->img);
	for (int i = params->begin; i < params->end; ++i)
	{
		int x = i % img.Width;
		int y = i / img.Width;

	}
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}