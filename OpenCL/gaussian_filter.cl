#pragma OPENCL EXTENSION cl_amd_printf : enable
#define OFF(x, y, ch) ((y) * pitch + (x) * 3 + ch)
__kernel void GaussianFilter(
	__global uchar *in, 
	__global uchar *out, 
	int width, int height, int pitch, 
	__global double *template
)
{
	__global uchar *src = in - pitch * (height - 1);
	__global uchar *dst = out - pitch * (height - 1);
	int x = get_global_id(0);
	int y = get_global_id(1);
	if(x >= width || y >= height)
		return;
	if(x < 1 || x >= width - 1 || y < 1 || y >= height - 1)
	{
		int offset = y * pitch + x * 3; 
		dst[offset + 0] = src[offset + 0];
		dst[offset + 1] = src[offset + 1];
		dst[offset + 2] = src[offset + 2];
		return;
	}
	for(int ch = 0; ch < 3; ++ch)
	{
		double val = 0.0;
		val += src[OFF(x - 1, y - 1, ch)] * template[0];
		val += src[OFF(x    , y - 1, ch)] * template[1];
		val += src[OFF(x + 1, y - 1, ch)] * template[2];
		val += src[OFF(x - 1, y    , ch)] * template[3];
		val += src[OFF(x    , y    , ch)] * template[4];
		val += src[OFF(x + 1, y    , ch)] * template[5];
		val += src[OFF(x - 1, y + 1, ch)] * template[6];
		val += src[OFF(x    , y + 1, ch)] * template[7];
		val += src[OFF(x + 1, y + 1, ch)] * template[8];
		if(val > 255.0)
			val = 255.0;
		if(val < 0.0)
			val = 0.0;
		dst[OFF(x, y, ch)] = val;
	}	
}
