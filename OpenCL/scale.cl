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
		int soffset = -(sp * sh) + y * sp + x * 3; // 3: bytepp
		int doffset = -(dp * dh) + fy * dp + fx * 3; 
		dst[doffset + 0] = src[soffset + 0];
		dst[doffset + 1] = src[soffset + 1];
		dst[doffset + 2] = src[soffset + 2];
		return;
	}
	int soffset = y * sp + x * 3; // 3: bytepp
	int doffset = fy * dp + fx * 3; 
	dst[doffset + 0] = src[soffset + 0];
	dst[doffset + 1] = src[soffset + 1];
	dst[doffset + 2] = src[soffset + 2];
}