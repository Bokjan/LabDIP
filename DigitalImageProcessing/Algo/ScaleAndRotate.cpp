#include "General.h"
#include "../Util/DisplayAgent.h"

// https://blog.demofox.org/2015/08/15/resizing-images-with-bicubic-interpolation/
static inline double BicubicCalc(double A, double B, double C, double D, double t)
{
	double a = -A / 2.0 + (3.0 * B) / 2.0 - (3.0 * C) / 2.0 + D / 2.0;
	double b = A - (5.0 * B) / 2.0 + 2.0 * C - D / 2.0;
	double c = -A / 2.0 + C / 2.0;
	double d = B;
	return a * t * t * t + b * t * t + c * t + d;
}

static void OnScaleFinished(ParallelParams *p)
{
	delete ((Algo::ScaleParams*)p->ctx)->src;
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
		double u = ix / (double)(img.Width - 1);
		double v = iy / (double)(img.Height - 1);
		double x = (u * src.Width) - 0.5;
		double y = (v * src.Height) - 0.5;
		int floorx = (int)x, floory = (int)y;
		double diffx = x - floorx, diffy = y - floory;

		// Handle the border
		if (floorx - 1 <= 0 || floorx + 2 >= src.Width - 1 || floory - 1 <= 0 || floory + 2 >= src.Height - 1)
		{
			floorx = floorx < 0 ? 0 : floorx;
			floorx = floorx >= src.Width ? src.Width - 1 : floorx;
			floory = floory < 0 ? 0 : floory;
			floory = floory >= src.Height ? src.Height - 1 : floory;
			byte *ptr = src.GetPixel(floorx, floory);
			img.SetPixel(ix, iy, ptr[0], ptr[1], ptr[2]);
			continue;
		}

		// 1st row
		auto p00 = src.GetPixel(floorx - 1, floory - 1);
		auto p10 = src.GetPixel(floorx + 0, floory - 1);
		auto p20 = src.GetPixel(floorx + 1, floory - 1);
		auto p30 = src.GetPixel(floorx + 2, floory - 1);

		// 2nd row
		auto p01 = src.GetPixel(floorx - 1, floory + 0);
		auto p11 = src.GetPixel(floorx + 0, floory + 0);
		auto p21 = src.GetPixel(floorx + 1, floory + 0);
		auto p31 = src.GetPixel(floorx + 2, floory + 0);

		// 3rd row
		auto p02 = src.GetPixel(floorx - 1, floory + 1);
		auto p12 = src.GetPixel(floorx + 0, floory + 1);
		auto p22 = src.GetPixel(floorx + 1, floory + 1);
		auto p32 = src.GetPixel(floorx + 2, floory + 1);

		// 4th row
		auto p03 = src.GetPixel(floorx - 1, floory + 2);
		auto p13 = src.GetPixel(floorx + 0, floory + 2);
		auto p23 = src.GetPixel(floorx + 1, floory + 2);
		auto p33 = src.GetPixel(floorx + 2, floory + 2);

		double result[3];
		for (int i = 0; i < 3; ++i)
		{
			double col0 = BicubicCalc(p00[i], p10[i], p20[i], p30[i], diffx);
			double col1 = BicubicCalc(p01[i], p11[i], p21[i], p31[i], diffx);
			double col2 = BicubicCalc(p02[i], p12[i], p22[i], p32[i], diffx);
			double col3 = BicubicCalc(p03[i], p13[i], p23[i], p33[i], diffx);
			result[i] = BicubicCalc(col0, col1, col2, col3, diffy);
			if (result[i] > 255.0)
				result[i] = 255;
			else if (result[i] < 0.0)
				result[i] = 0.0;
		}
		img.SetPixel(ix, iy, (byte)result[0], (byte)result[1], (byte)result[2]);
	}
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}


UINT Algo::ImageRotate(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	CImageWrapper img(params->img);
	for (int i = params->begin; i < params->end; ++i)
	{
		int x = i % img.Width;
		int y = i / img.Width;
		
	}
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}