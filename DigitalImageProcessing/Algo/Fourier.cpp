#include "General.h"
#include "../Util/DisplayAgent.h"

constexpr auto FOURIER_FACTOR = 16.0;
UINT Algo::ImageFourierTransform(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	auto source = (CImage*)(params->ctx);
	CImageWrapper img(source), dest(params->img);
	const double PI = acos(-1);
	const double DIM = img.Width * img.Height;

	for (int i = params->begin; i < params->end; ++i)
	{
		int u = i % img.Width;
		int v = i / img.Width;
		double real = 0.0, imag = 0.0;
		for (int y = 0; y < img.Height; ++y)
		{
			for (int x = 0; x < img.Width; ++x)
			{
				auto pixel = img.GetPixel(x, y);
				double gray = 0.299 * pixel[0] + 0.587 * pixel[1] + 0.114 * pixel[2];
				if ((x + y) & 1) // centralize
					gray = -gray;
				double A = 2 * PI * ((double)u * (double)x / (double)img.Width + (double)v * (double)y / (double)img.Height);
				real += gray * cos(A);
				imag -= gray * sin(A);
			}
		}
		double mag = sqrt(real * real + imag * imag);
		mag = FOURIER_FACTOR * log(mag + 1);
		if (mag < 0.0)
			mag = 0.0;
		else if (mag > 255.0)
			mag = 255.0;
		dest.SetPixel(u, v, (byte)mag, (byte)mag, (byte)mag);
	}
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}