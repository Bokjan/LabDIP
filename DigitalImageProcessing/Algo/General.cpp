#include "../stdafx.h"
#include "General.h"

ParallelParams* Algo::SplitTask(CImage *img, int thread)
{
	ParallelParams *ret = new ParallelParams[thread];
	int pixels = img->GetHeight() * img->GetWidth() / thread;
	for (int i = 0; i < thread; ++i)
	{
		ret[i].img = img;
		ret[i].begin = i * pixels;
		ret[i].end = i != thread - 1 ?
			(i + 1) * pixels - 1 :
			img->GetHeight() * img->GetWidth() - 1;
		ret[i].thctx = ret; // ��ʱ�أ�����ȫ���߳̽�����delete ParallelParams[]
	}
	return ret;
}