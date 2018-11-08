#include "General.h"
#include "../Util/DisplayAgent.h"
#include "../Util/CLAgent.h"

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

#define NOISE_FACTOR 0.05
UINT Algo::SaltAndPepperNoise(LPVOID _params)
{
	srand(static_cast<unsigned int>(time(nullptr)));
	auto params = (ParallelParams*)_params;
	CImageWrapper img(params->img);
	for (int i = params->begin; i < params->end; ++i)
	{
		int x = i % img.Width;
		int y = i / img.Width;
		if ((rand() / (double)RAND_MAX) <= NOISE_FACTOR)
		{
			byte val = (rand() & 0x1) ? 0 : 255;
			if (img.IsGray) // 灰度
				img.SetPixel(x, y, val);
			else
				img.SetPixel(x, y, val, val, val);
		}
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
			if (val < 0.0)
				val = 0.0;
			pixel[i] = (byte)val;
		}
	}
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}

UINT Algo::GaussianNoiseCL(LPVOID _params)
{
	srand((unsigned int)time(nullptr));
	auto params = (ParallelParams*)_params;
	auto gp = (GaussianParams*)(params->ctx);
	CImageWrapper img(params->img);

	DECLARE_CLA(cla);
	VERIFY(cla->LoadKernel("GaussianNoise"));
	DA->StartTick();
	auto imgmem = cla->CreateMemoryBuffer(img.MemSize(), img.MemStartAt());
	VERIFY(imgmem != nullptr);
	auto length = 3 * (img.Width * img.Height);
	auto random = new int[length];
	for (int i = 0; i < length; ++i)
	{
		int t;
		do
			t = rand();
		while (t == 0);
		random[i] = t;
	}
	auto rndmem = cla->CreateMemoryBuffer(sizeof(int) * length, random);
	VERIFY(rndmem != nullptr);
	cla->SetKernelArg(0, sizeof(cl_mem), &imgmem);
	cla->SetKernelArg(1, sizeof(cl_mem), &rndmem);
	cla->SetKernelArg(2, sizeof(int), &img.Width);
	cla->SetKernelArg(3, sizeof(int), &img.Height);
	cla->SetKernelArg(4, sizeof(int), &img.Pitch);
	cla->SetKernelArg(5, sizeof(double), &gp->mean);
	cla->SetKernelArg(6, sizeof(double), &gp->stddev);
	constexpr auto WORKDIM = 2;
	size_t localws[WORKDIM] = { 16, 16 };
	size_t globalws[WORKDIM] = {
		Algo::RoundUp(localws[0], img.Width),
		Algo::RoundUp(localws[1], img.Height),
	};
	VERIFY(cla->RunKernel(WORKDIM, localws, globalws));
	cla->ReadBuffer(imgmem, img.MemSize(), img.MemStartAt());
	cla->Cleanup();

	delete[] random;
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, params->wParam, (LPARAM)params);
	return 0;
}