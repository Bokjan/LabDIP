#include <complex>
#include <algorithm>
#include "General.h"
#include "../Util/DisplayAgent.h"
#include "../Util/CLAgent.h"

constexpr auto FOURIER_FACTOR = 14.0;
UINT Algo::FourierTransform(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	auto source = (CImage*)(params->ctx);
	CImageWrapper img(source), dest(params->img);
	const double PI = acos(-1);
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
		mag = std::clamp(mag, 0.0, 255.0);
		dest.SetPixel(u, v, (byte)mag, (byte)mag, (byte)mag);
	}
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, 1, (LPARAM)params);
	return 0;
}

UINT Algo::FourierTransformCL(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	auto source = (CImage*)(params->ctx);
	CImageWrapper src(source), dst(params->img);

	DECLARE_CLA(cla);
	VERIFY(cla->LoadKernel("D:\\Works\\LabDIP\\OpenCL\\fourier.cl", "Fourier"));
	auto inmem = cla->CreateMemoryBuffer(src.MemSize(), src.MemStartAt());
	VERIFY(inmem != nullptr);
	auto outmem = cla->CreateMemoryBuffer(dst.MemSize(), dst.MemStartAt());
	VERIFY(outmem != nullptr);
	cla->SetKernelArg(0, sizeof(inmem), &inmem);
	cla->SetKernelArg(1, sizeof(outmem), &outmem);
	cla->SetKernelArg(2, sizeof(int), &src.Width);
	cla->SetKernelArg(3, sizeof(int), &src.Height);
	cla->SetKernelArg(4, sizeof(int), &src.Pitch);
	constexpr auto WORKDIM = 2;
	size_t localws[WORKDIM] = { 4, 4 };
	size_t globalws[WORKDIM] = {
		Algo::RoundUp(localws[0], dst.Width),
		Algo::RoundUp(localws[1], dst.Height),
	};
	DA->StartTick();
	VERIFY(cla->RunKernel(WORKDIM, localws, globalws));
	cla->ReadBuffer(outmem, dst.MemSize(), dst.MemStartAt());
	cla->Cleanup();

	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, params->wParam, (LPARAM)params);
	return 0;
}

static inline int ReverseBit(int x, int len)
{
	int ret = 0;
	for (int i = 0; i < len; i++)
	{
		int mask = x & 1;
		ret <<= 1;
		ret = ret | mask;
		x >>= 1;
	}
	return ret;
}

static inline int ILOG2(int x)
{
	const static auto LN2 = log(2);
	return (int)round(log(x) / LN2);
}

// FFT is a single thread operation
#define OFFSET(x, y) (y * img.Width + x - params->begin)
UINT Algo::FastFourierTransform(LPVOID _params)
{
	auto params = (ParallelParams*)_params;
	auto source = (CImage*)(params->ctx);
	CImageWrapper src(source), img(params->img);
	const double PI = acos(-1);
	const double TWOPI = 2 * PI;
	const auto DIM = src.Width * src.Height;
	const int log2x = ILOG2(img.Width);
	const int log2y = ILOG2(img.Height);
	// Construct mappings
	auto xmap = new int[src.Width];
	auto ymap = new int[src.Height];
	for (int i = 0; i < src.Width; ++i)
		xmap[ReverseBit(i, log2x)] = i;
	for (int i = 0; i < src.Height; ++i)
		ymap[ReverseBit(i, log2y)] = i;
	// Construct W values
	using complex = std::complex<double>;
	auto xw = new complex[src.Width / 2];
	auto yw = new complex[src.Height / 2];
	for (int i = 0; i < src.Width / 2; ++i)
		xw[i] = complex(cos(TWOPI / (double)src.Width * (double)i), -sin(TWOPI / (double)src.Width * (double)i));
	for (int i = 0; i < src.Height / 2; ++i)
		yw[i] = complex(cos(TWOPI / (double)src.Height * (double)i), -sin(TWOPI / (double)src.Height * (double)i));
	// Transform image (grayscale, mapped)
	auto res = new complex[DIM];
	for (int j = 0; j < src.Height; ++j)
	{
		for (int i = 0; i < src.Width; ++i)
		{
			auto pixel = src.GetPixel(xmap[i], ymap[j]);
			double val = 0.299 * pixel[0] + 0.587 * pixel[1] + 0.114 * pixel[2];
			res[OFFSET(i, j)] = complex(std::clamp(val, 0.0, 255.0), 0.0);
		}
	}
	// Allocate mem
	auto xtmp = new complex[img.Width];
	auto ytmp = new complex[img.Height];
	// Calculate rows
	for (int row = 0; row < img.Height; ++row)
	{
		for (int i = 0; i < log2x; ++i)
		{
			int cnt = 0;
			int merge = 2 << i;
			for (int j = 0; j < img.Width; j += merge)
			{
				for (int k = 0; k < merge; ++k)
				{
					if (k < (merge >> 1))
					{
						int wpos = k * img.Width / merge;
						int offset = row * img.Width + k + j;
						xtmp[cnt++] = res[offset] + res[offset + (merge >> 1)] * xw[wpos];
					}
					else
					{
						int wpos = (k - (merge >> 1)) * img.Width / merge;
						int offset = row * img.Width + k + j - (merge >> 1);
						xtmp[cnt++] = res[offset] + (complex(-1.0, 0.0) * res[offset + (merge >> 1)] * xw[wpos]);
					}
				}
			}
			for (int tt = 0; tt < img.Width; ++tt)
				res[row * img.Width + tt] = xtmp[tt];
		}
	}
	// Calculate columns
	for (int col = 0; col < img.Width; ++col)
	{
		for (int i = 0; i < log2y; ++i)
		{
			int cnt = 0;
			int merge = 2 << i;
			for (int j = 0; j < img.Height; j += merge)
			{
				for (int k = 0; k < merge; ++k)
				{
					if (k < (merge >> 1))
					{
						int wpos = k * img.Height / merge;
						int offset = (j + k) * img.Width + col;
						ytmp[cnt++] = res[offset] + res[offset + (merge >> 1) * img.Width] * yw[wpos];
					}
					else
					{
						int wpos = (k - (merge >> 1)) * img.Height / merge;
						int offset = (j + k - (merge >> 1)) * img.Width + col;
						ytmp[cnt++] = res[offset] * (complex(-1.0, 0.0) * res[offset + (merge >> 1) * img.Width] * yw[wpos]);
					}
				}
			}
			for (int tt = 0; tt < img.Height; ++tt)
				res[tt * img.Width + col] = ytmp[tt];
		}
	}
	// Fill pixels
	auto vals = new double[DIM];
	double max = DBL_MIN, min = DBL_MAX;
	for (int j = 0; j < img.Height; ++j)
	{
		for (int i = 0; i < img.Width; ++i)
		{
			const auto offset = OFFSET(i, j);
			auto magnitude = std::abs(res[offset]);
			vals[offset] = log(magnitude + 1);
			if (vals[offset] > max)
				max = vals[offset];
			if (vals[offset] < min)
				min = vals[offset];
		}
	}
	for (int j = 0; j < img.Height; ++j)
	{
		for (int i = 0; i < img.Width; ++i)
		{
			const auto offset = OFFSET(i, j);
			auto t = vals[offset];
			t = t - min * 255.0 / (max - min);
			img.SetPixel(i, j, (byte)t, (byte)t, (byte)t);
		}
	}
	// Release allocated memory
	delete[] xmap; delete[] ymap;
	delete[] xw; delete[] yw;
	delete[] res;
	delete[] xtmp; delete[] ytmp;
	delete[] vals;
	PostMessageW(DA->HWnd, WM_USER_EXECUTE_FINISHED, params->wParam, (LPARAM)params);
	return 0;
}
#undef OFFSET