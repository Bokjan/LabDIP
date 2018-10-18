#include "General.h"
#include "../Util/DisplayAgent.h"
#include <algorithm>

UINT Algo::MedianFilter(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	auto length = params->end - params->begin;
	CImageWrapper img(params->img);
	byte *br, *bg, *bb;
	br = new byte[length];
	bg = new byte[length];
	bb = new byte[length];
	const int SIZE = 9, MID = 4;
	byte r[SIZE], g[SIZE], b[SIZE];
	for (int i = params->begin; i < params->end; ++i)
	{
		int x = i % img.Width;
		int y = i / img.Width;
		if (x <= 0 || x >= img.Width - 1 || y <= 0 || y >= img.Height - 1)
			continue; // skip the border

#define FILL_BUFFER(_idx, _x, _y) { \
byte *_t = img.GetPixel(_x, _y); \
r[_idx]=*_t; g[_idx]=*(_t+1); b[_idx]=*(_t+2); }
		FILL_BUFFER(0, x - 1, y - 1); FILL_BUFFER(1, x, y - 1); FILL_BUFFER(2, x + 1, y - 1);
		FILL_BUFFER(3, x - 1, y);     FILL_BUFFER(4, x, y);     FILL_BUFFER(5, x + 1, y);
		FILL_BUFFER(6, x - 1, y + 1); FILL_BUFFER(7, x, y + 1); FILL_BUFFER(8, x + 1, y - 1);
#undef FILL_BUFFER

		std::sort(r, r + SIZE);
		std::sort(g, g + SIZE);
		std::sort(b, b + SIZE);

		br[i - params->begin] = r[MID];
		bg[i - params->begin] = g[MID];
		bb[i - params->begin] = b[MID];
	}
	for (int i = 0; i < length; ++i) 
	{
		int x = (i + params->begin) % img.Width;
		int y = (i + params->begin) / img.Width;
		img.SetPixel(x, y, br[i], bg[i], bb[i]);
	}
	delete[] br;
	delete[] bg;
	delete[] bb;
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}

UINT Algo::ArithMeanFilter(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	CImageWrapper img(params->img), src((CImage*)params->ctx);
	const int TEMPLATE_SIZE = 3 * 3;
	for (int idx = params->begin; idx < params->end; ++idx)
	{
		int x = idx % img.Width;
		int y = idx / img.Width;
		// skip the border
		if (x < 1 || y < 1 || x >= img.Width - 1 || y >= img.Height - 1)
		{
			img.SetPixel(x, y, src.GetPixel(x, y));
			continue;
		}
		int r, g, b;
		r = g = b = 0;
#define ACCUMULATE(_x, _y) { \
auto _t = src.GetPixel(_x, _y); \
r += _t[0]; g += _t[1]; b += _t[2]; }
		ACCUMULATE(x - 1, y - 1); ACCUMULATE(x, y - 1); ACCUMULATE(x + 1, y - 1);
		ACCUMULATE(x - 1, y);     ACCUMULATE(x, y);     ACCUMULATE(x + 1, y);
		ACCUMULATE(x - 1, y + 1); ACCUMULATE(x, y + 1); ACCUMULATE(x + 1, y - 1);
#undef ACCUMULATE
		img.SetPixel(x, y, (byte)(r / TEMPLATE_SIZE), (byte)(g / TEMPLATE_SIZE), (byte)(b / TEMPLATE_SIZE));
	}
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}

static void OnGaussianFilterFinished(ParallelParams *p)
{
	auto gp = (Algo::GaussianParams*)p->ctx;
	delete gp->src;
}

static void GetGaussianTemplate(double t[3][3], double stddev)
{
	const int center = 1; // [[0, 1, 2], [0, 1, 2], [0, 1, 2]], center is (1, 1)
	double total = 0;
	static const double PI = acos(-1);
	for (int i = 0; i < 3; ++i)
	{
		double xsq = pow(i - center, 2.0);
		for (int j = 0; j < 3; ++j)
		{
			double ysq = pow(j - center, 2.0);
			double f = 1 / (2.0 * PI * stddev);
			double e = exp(-(xsq + ysq) / (2.0 * stddev * stddev));
			t[i][j] = f * e;
			total += t[i][j];
		}
	}
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			t[i][j] /= total;
}

UINT Algo::GaussianFilter(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	auto gp = (GaussianParams*)params->ctx;
	CImageWrapper img(params->img), src((CImage*)gp->src);
	const int SIZE = 3;
	double m[SIZE][SIZE];
	GetGaussianTemplate(m, gp->stddev);
	for (int idx = params->begin; idx < params->end; ++idx)
	{
		int x = idx % img.Width;
		int y = idx / img.Width;
		// skip the border
		if (x < 1 || y < 1 || x >= img.Width - 1 || y >= img.Height - 1)
		{
			img.SetPixel(x, y, src.GetPixel(x, y));
			continue;
		}
		double r, g, b;
		r = g = b = 0.0;
#define ACCUMULATE(_x, _y, _a, _b) { \
auto _t = src.GetPixel(_x, _y); \
r += (double)_t[0]*m[_a][_b]; \
g += (double)_t[1]*m[_a][_b]; \
b += (double)_t[2]*m[_a][_b]; }
		ACCUMULATE(x - 1, y - 1, 0, 0); ACCUMULATE(x, y - 1, 1, 0); ACCUMULATE(x + 1, y - 1, 2, 0);
		ACCUMULATE(x - 1, y   ,  0, 1); ACCUMULATE(x, y    , 1, 1); ACCUMULATE(x + 1, y    , 2, 1);
		ACCUMULATE(x - 1, y + 1, 0, 2); ACCUMULATE(x, y + 1, 1, 2); ACCUMULATE(x + 1, y - 1, 2, 2);
#undef ACCUMULATE
#define CLAMP(v) {if(v>255.0)v=255.0;else if(v<0.0)v=0.0;}
		CLAMP(r); CLAMP(g); CLAMP(b);
#undef CLAMP
		img.SetPixel(x, y, (byte)r, (byte)g, (byte)b);
	}
	params->cb = OnGaussianFilterFinished;
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}
