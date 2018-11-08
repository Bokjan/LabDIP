#include <algorithm>
#include "General.h"
#include "../Util/DisplayAgent.h"
#include "../Util/CLAgent.h"

// https://blog.demofox.org/2015/08/15/resizing-images-with-bicubic-interpolation/
static inline double BicubicHermite(double A, double B, double C, double D, double t)
{
	double a = -A / 2.0 + (3.0 * B) / 2.0 - (3.0 * C) / 2.0 + D / 2.0;
	double b = A - (5.0 * B) / 2.0 + 2.0 * C - D / 2.0;
	double c = -A / 2.0 + C / 2.0;
	double d = B;
	return a * t * t * t + b * t * t + c * t + d;
}

static inline double BicubicWeight(double x)
{
	constexpr double a = -0.5;
	x = std::abs(x);
	if (x < 1.0)
		return (a + 2.0)*x*x*x - (a + 3.0)*x*x + 1.0;
	else if (x < 2.0)
		return a * x*x*x - 5.0*a * x*x + 8.0*a * x - 4.0 * a;
	return 0.0;
}

UINT Algo::ImageScale(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	auto sp = (Algo::ScaleParams*)(params->ctx);
	CImageWrapper img(params->img), src(sp->src);
	for (int i = params->begin; i < params->end; ++i)
	{
		int ix = i % img.Width;
		int iy = i / img.Width;
		double x = ix / ((double)img.Width / src.Width);
		double y = iy / ((double)img.Height / src.Height);
		int fx = (int)x, fy = (int)y;

		// Handle the border
		if (fx - 1 <= 0 || fx + 2 >= src.Width - 1 || fy - 1 <= 0 || fy + 2 >= src.Height - 1)
		{
			fx = fx < 0 ? 0 : fx;
			fx = fx >= src.Width ? src.Width - 1 : fx;
			fy = fy < 0 ? 0 : fy;
			fy = fy >= src.Height ? src.Height - 1 : fy;
			img.SetPixel(ix, iy, src.GetPixel(fx, fy));
			continue;
		}
		// Calc w
		double wx[4], wy[4];
		wx[0] = BicubicWeight(fx - 1 - x);
		wx[1] = BicubicWeight(fx + 0 - x);
		wx[2] = BicubicWeight(fx + 1 - x);
		wx[3] = BicubicWeight(fx + 2 - x);
		wy[0] = BicubicWeight(fy - 1 - y);
		wy[1] = BicubicWeight(fy + 0 - y);
		wy[2] = BicubicWeight(fy + 1 - y);
		wy[3] = BicubicWeight(fy + 2 - y);
		// Get pixels
		byte *p[4][4];
#define FILLPX(x, y, i, j) p[i][j]=src.GetPixel(x, y)
		FILLPX(fx - 1, fy - 1, 0, 0);
		FILLPX(fx - 1, fy + 0, 0, 1);
		FILLPX(fx - 1, fy + 1, 0, 2);
		FILLPX(fx - 1, fy + 2, 0, 3);
		FILLPX(fx + 0, fy - 1, 1, 0);
		FILLPX(fx + 0, fy + 0, 1, 1);
		FILLPX(fx + 0, fy + 1, 1, 2);
		FILLPX(fx + 0, fy + 2, 1, 3);
		FILLPX(fx + 1, fy - 1, 2, 0);
		FILLPX(fx + 1, fy + 0, 2, 1);
		FILLPX(fx + 1, fy + 1, 2, 2);
		FILLPX(fx + 1, fy + 2, 2, 3);
		FILLPX(fx + 2, fy - 1, 3, 0);
		FILLPX(fx + 2, fy + 0, 3, 1);
		FILLPX(fx + 2, fy + 1, 3, 2);
		FILLPX(fx + 2, fy + 2, 3, 3);
#undef FILLPX
		double rgb[3];
		rgb[0] = rgb[1] = rgb[2] = 0.0;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
			{
				rgb[0] += p[i][j][0] * wx[i] * wy[j];
				rgb[1] += p[i][j][1] * wx[i] * wy[j];
				rgb[2] += p[i][j][2] * wx[i] * wy[j];
			}
		for (int i = 0; i < 3; ++i)
			rgb[i] = std::clamp(rgb[i], 0.0, 255.0);
		img.SetPixel(ix, iy, (byte)rgb[0], (byte)rgb[1], (byte)rgb[2]);
	}
	params->cb = [](ParallelParams *p)
	{
		delete ((Algo::ScaleParams*)p->ctx)->src;
	};
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, params->wParam, (LPARAM)params);
	return 0;
}

UINT Algo::ImageRotate(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	auto rp = (Algo::RotateParams*)(params->ctx);
	CImageWrapper img(params->img);
	CImageWrapper src(rp->src);
	const double sina = sin(rp->angle), cosa = cos(rp->angle);
	Algo::vec2<double> ncenter = { img.Width / 2.0, img.Height / 2.0 };
	Algo::vec2<double> ocenter = { src.Width / 2.0, src.Height / 2.0 };

	for (int i = params->begin; i < params->end; ++i)
	{
		int x = i % img.Width;
		int y = i / img.Width;
		int xx = static_cast<int>(x - ncenter.x);
		int yy = static_cast<int>(y - ncenter.y);
		double oldx = xx * cosa - yy * sina + ocenter.x;
		double oldy = xx * sina + yy * cosa + ocenter.y;
		int iox = (int)oldx, ioy = (int)oldy;

		// out of interpolation border
		if (iox <= 1 || iox + 2 >= src.Width - 1 || ioy <= 1 || ioy + 2 >= src.Height)
		{
			// but, still in the original image
			if (iox >= 0 && iox < src.Width && ioy>=0 && ioy < src.Height)
				img.SetPixel(x, y, src.GetPixel(iox, ioy));
			continue;
		}

		// Bicubic interpolation
		// 1st row
		auto p00 = src.GetPixel(iox - 1, ioy - 1);
		auto p10 = src.GetPixel(iox + 0, ioy - 1);
		auto p20 = src.GetPixel(iox + 1, ioy - 1);
		auto p30 = src.GetPixel(iox + 2, ioy - 1);

		// 2nd row
		auto p01 = src.GetPixel(iox - 1, ioy + 0);
		auto p11 = src.GetPixel(iox + 0, ioy + 0);
		auto p21 = src.GetPixel(iox + 1, ioy + 0);
		auto p31 = src.GetPixel(iox + 2, ioy + 0);

		// 3rd row
		auto p02 = src.GetPixel(iox - 1, ioy + 1);
		auto p12 = src.GetPixel(iox + 0, ioy + 1);
		auto p22 = src.GetPixel(iox + 1, ioy + 1);
		auto p32 = src.GetPixel(iox + 2, ioy + 1);

		// 4th row
		auto p03 = src.GetPixel(iox - 1, ioy + 2);
		auto p13 = src.GetPixel(iox + 0, ioy + 2);
		auto p23 = src.GetPixel(iox + 1, ioy + 2);
		auto p33 = src.GetPixel(iox + 2, ioy + 2);

		double result[3];
		for (int i = 0; i < 3; ++i)
		{
			double col0 = BicubicHermite(p00[i], p10[i], p20[i], p30[i], oldx - iox);
			double col1 = BicubicHermite(p01[i], p11[i], p21[i], p31[i], oldx - iox);
			double col2 = BicubicHermite(p02[i], p12[i], p22[i], p32[i], oldx - iox);
			double col3 = BicubicHermite(p03[i], p13[i], p23[i], p33[i], oldx - iox);
			result[i] = BicubicHermite(col0, col1, col2, col3, oldy - ioy);
			if (result[i] > 255.0)
				result[i] = 255.0;
			if (result[i] < 0.0)
				result[i] = 0.0;
		}
		img.SetPixel(x, y, (byte)result[0], (byte)result[1], (byte)result[2]);
	}
	params->cb = [](ParallelParams *p)
	{
		delete ((Algo::RotateParams*)p->ctx)->src;
	};
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}

UINT Algo::ImageScaleCL(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	auto sp = (Algo::ScaleParams*)(params->ctx);
	CImageWrapper dst(params->img), src(sp->src);

	DECLARE_CLA(cla);
	VERIFY(cla->LoadKernel("Scale"));
	auto inmem = cla->CreateMemoryBuffer(src.MemSize(), src.MemStartAt());
	VERIFY(inmem != nullptr);
	auto outmem = cla->CreateMemoryBuffer(dst.MemSize(), dst.MemStartAt());
	VERIFY(outmem != nullptr);
	cla->SetKernelArg(0, sizeof(inmem), &inmem);
	cla->SetKernelArg(1, sizeof(outmem), &outmem);
	cla->SetKernelArg(2, sizeof(int), &src.Width);
	cla->SetKernelArg(3, sizeof(int), &src.Height);
	cla->SetKernelArg(4, sizeof(int), &dst.Width);
	cla->SetKernelArg(5, sizeof(int), &dst.Height);
	cla->SetKernelArg(6, sizeof(int), &src.Pitch);
	cla->SetKernelArg(7, sizeof(int), &dst.Pitch);
	constexpr auto WORKDIM = 2;
	size_t localws[WORKDIM] = { 16, 16 };
	size_t globalws[WORKDIM] = {
		Algo::RoundUp(localws[0], dst.Width),
		Algo::RoundUp(localws[1], dst.Height),
	};
	DA->StartTick();
	VERIFY(cla->RunKernel(WORKDIM, localws, globalws));
	cla->ReadBuffer(outmem, dst.MemSize(), dst.MemStartAt());
	cla->Cleanup();

	params->cb = [](ParallelParams *p)
	{
		delete ((Algo::ScaleParams*)p->ctx)->src;
	};
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, params->wParam, (LPARAM)params);
	return 0;
}

UINT Algo::ImageRotateCL(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	auto rp = (Algo::RotateParams*)(params->ctx);
	CImageWrapper dst(params->img);
	CImageWrapper src(rp->src);

	DECLARE_CLA(cla);
	VERIFY(cla->LoadKernel("Rotate"));
	auto inmem = cla->CreateMemoryBuffer(src.MemSize(), src.MemStartAt());
	VERIFY(inmem != nullptr);
	auto outmem = cla->CreateMemoryBuffer(dst.MemSize(), dst.MemStartAt());
	VERIFY(outmem != nullptr);
	cla->SetKernelArg(0, sizeof(inmem), &inmem);
	cla->SetKernelArg(1, sizeof(outmem), &outmem);
	cla->SetKernelArg(2, sizeof(int), &src.Width);
	cla->SetKernelArg(3, sizeof(int), &src.Height);
	cla->SetKernelArg(4, sizeof(int), &dst.Width);
	cla->SetKernelArg(5, sizeof(int), &dst.Height);
	cla->SetKernelArg(6, sizeof(int), &src.Pitch);
	cla->SetKernelArg(7, sizeof(int), &dst.Pitch);
	cla->SetKernelArg(8, sizeof(double), &rp->angle);
	constexpr auto WORKDIM = 2;
	size_t localws[WORKDIM] = { 16, 16 };
	size_t globalws[WORKDIM] = {
		Algo::RoundUp(localws[0], dst.Width),
		Algo::RoundUp(localws[1], dst.Height),
	};
	DA->StartTick();
	VERIFY(cla->RunKernel(WORKDIM, localws, globalws));
	cla->ReadBuffer(outmem, dst.MemSize(), dst.MemStartAt());
	cla->Cleanup();

	params->cb = [](ParallelParams *p)
	{
		delete ((Algo::RotateParams*)p->ctx)->src;
	};
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, params->wParam, (LPARAM)params);
	return 0;
}