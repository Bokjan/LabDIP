#include "General.h"
#include "../Util/DisplayAgent.h"

// https://github.com/Bokjan/Radiom/blob/master/distribution/normal.c
// 我 抄 我 自 己
static double BoxMullerGenerator(double mean, double stddev)
{
	static const double twopi = 2.0 * acos(-1);
	double u1, u2;
	static double z0, z1;
	static bool generate = false;
	generate = !generate;
	if (!generate)
		return z1 * stddev + mean;
	do
	{
		u1 = (double)rand() / RAND_MAX;
		u2 = (double)rand() / RAND_MAX;
	} while (u1 <= DBL_MIN);
	z0 = sqrt(-2.0 * log(u1)) * cos(twopi * u2);
	z1 = sqrt(-2.0 * log(u1)) * sin(twopi * u2);
	return z0 * stddev + mean;
}

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
			if (img.IsGray) // 灰度
				img.SetPixel(x, y, val);
			else
				img.SetPixel(x, y, val, val, val);
		}
#undef NOISE_FACTOR
	}
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}

UINT Algo::GaussianNoise(LPVOID _params)
{
	srand((unsigned int)time(nullptr));
	auto params = (ParallelParams*)_params;
	auto gp = (GaussianParams*)(params->ctx);
	CImageWrapper img(params->img);
	for (int idx = params->begin; idx < params->end; ++idx)
	{
		int x = idx % img.Width;
		int y = idx / img.Width;
		auto pixel = img.GetPixel(x, y);
		for (int i = 0; i < 3; ++i)
		{
			double val = pixel[i] + BoxMullerGenerator(gp->mean, gp->stddev);
			if (val > 255.0)
				val = 255.0;
			else if (val < 0.0)
				val = 0.0;
			pixel[i] = (byte)val;
		}
	}
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}