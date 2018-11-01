#pragma OPENCL EXTENSION cl_amd_printf : enable
#define PI (3.14159265358)
#define TWOPI (6.28318530717)
#define FOURIER_FACTOR (14.0)
#define OFF(x, y, ch) ((y) * pitch + (x) * 3 + ch)
__kernel void Fourier(__global uchar *in, __global uchar *out, int width, int height, int pitch)
{
	__global uchar *src = in - pitch * (height - 1);
	__global uchar *dst = out - pitch * (height - 1);
	int u = get_global_id(0);
	int v = get_global_id(1);
	if(u >= width || v >= height)
		return;
	double real = 0.0, imag = 0.0;
	for(int y = 0; y < height; ++y)
	for(int x = 0; x < width; ++x)
	{
		double gray = 0.299 * src[OFF(x, y, 0)] + 0.587 * src[OFF(x, y, 1)] + 0.114 * src[OFF(x, y, 2)];
		if((x + y) & 1)
			gray = -gray;
		double A = TWOPI * ((double)u * (double)x / (double)width + (double)v * (double)y / (double)height);
		real += gray * cos(A);
		imag -= gray * sin(A);
	}
	double mag = sqrt(real * real + imag * imag);
	mag = FOURIER_FACTOR * log(mag + 1);
	if(mag > 255.0)
		mag = 255.0;
	else if (mag < 0.0)
		mag = 0.0;
	dst[OFF(u, v, 0)] = mag;
	dst[OFF(u, v, 1)] = mag;
	dst[OFF(u, v, 2)] = mag;
}
