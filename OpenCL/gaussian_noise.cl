#pragma OPENCL EXTENSION cl_amd_printf : enable
#define OFF(x, y, ch) ((y) * pitch + (x) * 3 + ch)
#define TWOPI (6.28318530717)
double CLMAP0255(double x)
{
	if(x < 0.0)
		return 0.0;
	if(x > 255.0)
		return 255.0;
	return x;
}

__kernel void GaussianNoise(__global uchar *imem, __global double *rmem, int width, int height, int pitch, double mean, double stddev)
{
	int x = get_global_id(0);
	int y = get_global_id(1);
	if(x >= width || y >= height)
		return;
	__global uchar *img = imem - pitch * (height - 1);
	__global double *rnd = rmem + (y * width + x) * 3;
	double r1, r2, r3;
	if((y * width + x) & 1 == 0)
	{
		r1 = sqrt(-2.0 * log(rnd[0])) * cos(TWOPI * rnd[1]) * stddev + mean;
		r2 = sqrt(-2.0 * log(rnd[0])) * sin(TWOPI * rnd[1]) * stddev + mean;
		r3 = sqrt(-2.0 * log(rnd[2])) * cos(TWOPI * rnd[3]) * stddev + mean;
	}
	else
	{
		r1 = sqrt(-2.0 * log(rnd[-1])) * sin(TWOPI * rnd[0]) * stddev + mean;
		r2 = sqrt(-2.0 * log(rnd[1])) * cos(TWOPI * rnd[2]) * stddev + mean;
		r3 = sqrt(-2.0 * log(rnd[1])) * sin(TWOPI * rnd[2]) * stddev + mean;
	}
	img[OFF(x, y, 0)] = CLMAP0255(img[OFF(x, y, 0)] + r1);
	img[OFF(x, y, 1)] = CLMAP0255(img[OFF(x, y, 1)] + r2);
	img[OFF(x, y, 2)] = CLMAP0255(img[OFF(x, y, 2)] + r3);
}
