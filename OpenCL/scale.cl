double BicubicWeight(double x)
{
	double a = -0.5;
	if(x < 0)
		x = -x;
	if (x < 1.0)
		return (a + 2.0)*x*x*x - (a + 3.0)*x*x + 1.0;
	else if (x < 2.0)
		return a * x*x*x - 5.0*a * x*x + 8.0*a * x - 4.0 * a;
	return 0.0;
}
#pragma OPENCL EXTENSION cl_amd_printf : enable
#define SOFF(x, y, ch) ((y) * sp + (x) * 3 + ch)
#define DOFF(x, y, ch) ((y) * dp + (x) * 3 + ch)
__kernel void Scale(__global uchar *in, __global uchar *out, int sw, int sh, int dw, int dh, int sp, int dp)
{
	__global uchar *src = in - sp * (sh - 1);
	__global uchar *dst = out - dp * (dh - 1);
	int x = get_global_id(0);
	int y = get_global_id(1);
	if(x >= dw || y >= dh)
		return;
	float sx = x / ((float)dw / (float)sw);
	float sy = y / ((float)dh / (float)sh);
	int fx = (int)sx;
	int fy = (int)sy;
	if(fx - 1 <= 0 || fx + 2 >= sw - 1 || fy - 1 <= 0 || fy + 2 >= sh - 1)
	{
		fx = fx < 0 ? 0 : fx;
		fx = fx >= sw ? sw - 1 : fx;
		fy = fy < 0 ? 0 : fy;
		fy = fy >= sh ? sh - 1 : fy;
		int soffset = fy * sp + fx * 3; 
		int doffset = y * dp + x * 3; // 3: bytepp
		dst[doffset + 0] = src[soffset + 0];
		dst[doffset + 1] = src[soffset + 1];
		dst[doffset + 2] = src[soffset + 2];
		return;
	}
	// Calculate W
	double wx[4], wy[4];
	wx[0] = BicubicWeight(fx - 1 - sx);
	wx[1] = BicubicWeight(fx + 0 - sx);
	wx[2] = BicubicWeight(fx + 1 - sx);
	wx[3] = BicubicWeight(fx + 2 - sx);
	wy[0] = BicubicWeight(fy - 1 - sy);
	wy[1] = BicubicWeight(fy + 0 - sy);
	wy[2] = BicubicWeight(fy + 1 - sy);
	wy[3] = BicubicWeight(fy + 2 - sy);
	for(int ch = 0; ch < 3; ++ch)
	{
		double val = 0;
		val += src[SOFF(fx - 1, fy - 1, ch)] * wx[0] * wy[0];
		val += src[SOFF(fx - 1, fy + 0, ch)] * wx[0] * wy[1];
		val += src[SOFF(fx - 1, fy + 1, ch)] * wx[0] * wy[2];
		val += src[SOFF(fx - 1, fy + 2, ch)] * wx[0] * wy[3];
		val += src[SOFF(fx + 0, fy - 1, ch)] * wx[1] * wy[0];
		val += src[SOFF(fx + 0, fy + 0, ch)] * wx[1] * wy[1];
		val += src[SOFF(fx + 0, fy + 1, ch)] * wx[1] * wy[2];
		val += src[SOFF(fx + 0, fy + 2, ch)] * wx[1] * wy[3];
		val += src[SOFF(fx + 1, fy - 1, ch)] * wx[2] * wy[0];
		val += src[SOFF(fx + 1, fy + 0, ch)] * wx[2] * wy[1];
		val += src[SOFF(fx + 1, fy + 1, ch)] * wx[2] * wy[2];
		val += src[SOFF(fx + 1, fy + 2, ch)] * wx[2] * wy[3];
		val += src[SOFF(fx + 2, fy - 1, ch)] * wx[3] * wy[0];
		val += src[SOFF(fx + 2, fy + 0, ch)] * wx[3] * wy[1];
		val += src[SOFF(fx + 2, fy + 1, ch)] * wx[3] * wy[2];
		val += src[SOFF(fx + 2, fy + 2, ch)] * wx[3] * wy[3];
		if(val > 255.0)
			val = 255.0;
		if(val < 0.0)
			val = 0.0;
		dst[DOFF(x, y, ch)] = (uchar)val;
	}
}
