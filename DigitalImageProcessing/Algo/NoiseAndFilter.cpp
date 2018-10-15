#include "General.h"
#include "../Util/DisplayAgent.h"
#include <algorithm>
#define NOISE_FACTOR (0.1)

UINT Algo::SaltAndPepperNoise(LPVOID _params)
{
	srand(static_cast<unsigned int>(time(nullptr)));
	ParallelParams *params = (ParallelParams*)_params;
	CImageWrapper img(params->img);
	for (int i = params->begin; i < params->end; ++i)
	{
		int x = i % img.Width;
		int y = i / img.Width;
		if ((rand() / (double)RAND_MAX) <= NOISE_FACTOR)
		{
			byte val = (rand() & 0x1) ? 0 : 255;
			if (img.IsGray) // »Ò¶È
				img.SetPixel(x, y, val);
			else
				img.SetPixel(x, y, val, val, val);
		}
	}
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}

UINT Algo::MedianFilter(LPVOID _params)
{
	ParallelParams *params = (ParallelParams*)_params;
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
		FILL_BUFFER(0, x - 1, y - 1) FILL_BUFFER(1, x, y - 1) FILL_BUFFER(2, x + 1, y - 1)
		FILL_BUFFER(3, x - 1, y)     FILL_BUFFER(4, x, y)     FILL_BUFFER(5, x + 1, y)
		FILL_BUFFER(6, x - 1, y + 1) FILL_BUFFER(7, x, y + 1) FILL_BUFFER(8, x + 1, y - 1)

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
		img.SetPixel(x, y, br[i], bg[i], bg[i]);
	}
	delete[] br;
	delete[] bg;
	delete[] bb;
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}