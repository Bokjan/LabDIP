#include "General.h"
#include "../Util/DisplayAgent.h"

// https://blog.demofox.org/2015/08/15/resizing-images-with-bicubic-interpolation/
static inline double CalcBicubicWeight(double A, double B, double C, double D, double t)
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

static void OnRotateFinished(ParallelParams *p)
{
	delete ((Algo::RotateParams*)p->ctx)->src;
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
			img.SetPixel(ix, iy, src.GetPixel(floorx, floory));
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
			double col0 = CalcBicubicWeight(p00[i], p10[i], p20[i], p30[i], diffx);
			double col1 = CalcBicubicWeight(p01[i], p11[i], p21[i], p31[i], diffx);
			double col2 = CalcBicubicWeight(p02[i], p12[i], p22[i], p32[i], diffx);
			double col3 = CalcBicubicWeight(p03[i], p13[i], p23[i], p33[i], diffx);
			result[i] = CalcBicubicWeight(col0, col1, col2, col3, diffy);
			if (result[i] > 255.0)
				result[i] = 255.0;
			else if (result[i] < 0.0)
				result[i] = 0.0;
		}
		img.SetPixel(ix, iy, (byte)result[0], (byte)result[1], (byte)result[2]);
	}
	params->cb = OnScaleFinished;
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
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
			double col0 = CalcBicubicWeight(p00[i], p10[i], p20[i], p30[i], oldx - iox);
			double col1 = CalcBicubicWeight(p01[i], p11[i], p21[i], p31[i], oldx - iox);
			double col2 = CalcBicubicWeight(p02[i], p12[i], p22[i], p32[i], oldx - iox);
			double col3 = CalcBicubicWeight(p03[i], p13[i], p23[i], p33[i], oldx - iox);
			result[i] = CalcBicubicWeight(col0, col1, col2, col3, oldy - ioy);
			if (result[i] > 255.0)
				result[i] = 255.0;
			else if (result[i] < 0.0)
				result[i] = 0.0;
		}
		img.SetPixel(x, y, (byte)result[0], (byte)result[1], (byte)result[2]);
	}
	params->cb = OnRotateFinished;
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}