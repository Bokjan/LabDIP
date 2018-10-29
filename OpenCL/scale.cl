__kernel void Scale(__global uchar *in, __global uchar *out, int sw, int sh, int dw, int dh)
{
	/*int x = get_global_id(0);
	int y = get_global_id(1);
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
		out[y * dw + x] = in[fy * sw + fx];
		return;
	}
	out[y * dw + x] = in[fy * sw + fx];*/
}