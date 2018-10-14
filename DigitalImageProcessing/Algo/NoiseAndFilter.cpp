#include "General.h"
#define NOISE (0.2)

UINT Algo::SaltAndPepperNoise(LPVOID _params)
{
	ParallelParams *params = (ParallelParams*)_params;
	int maxWidth = params->img->GetWidth();
	int maxHeight = params->img->GetHeight();

	int startIndex = params->begin;
	int endIndex = params->end;
	byte* pRealData = (byte*)params->img->GetBits();
	int bitCount = params->img->GetBPP() / 8;
	int pit = params->img->GetPitch();

	for (int i = startIndex; i <= endIndex; ++i)
	{
		int x = i % maxWidth;
		int y = i / maxWidth;
		if ((rand() % 1000) * 0.001 < NOISE)
		{
			int value = 0;
			if (rand() % 1000 < 500)
			{
				value = 0;
			}
			else
			{
				value = 255;
			}
			if (bitCount == 1)
			{
				*(pRealData + pit * y + x * bitCount) = value;
			}
			else
			{
				*(pRealData + pit * y + x * bitCount) = value;
				*(pRealData + pit * y + x * bitCount + 1) = value;
				*(pRealData + pit * y + x * bitCount + 2) = value;
			}
		}
	}
	return 0;
}

UINT Algo::MedianFilter(LPVOID params)
{
	return 0;
}