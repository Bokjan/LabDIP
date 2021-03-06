// CTabScaleRotateDlg.cpp: 实现文件
//

#include "../stdafx.h"
#include "../DigitalImageProcessing.h"
#include "../Util/DisplayAgent.h"
#include "../Algo/General.h"
#include "TabScaleRotateDlg.h"
#include "afxdialogex.h"


// CTabScaleRotateDlg 对话框

IMPLEMENT_DYNAMIC(CTabScaleRotateDlg, CDialogEx)

CTabScaleRotateDlg::CTabScaleRotateDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_SCALEROTATE_DIALOG, pParent)
{

}

CTabScaleRotateDlg::~CTabScaleRotateDlg()
{
}

void CTabScaleRotateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SRFUNCTION, mSRFunction);
	DDX_Control(pDX, IDC_EDIT_SRPARAMETER, mSRParameter);
}

BOOL CTabScaleRotateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	mSRFunction.InsertString(0, _T("缩放"));
	mSRFunction.InsertString(1, _T("旋转"));
	mSRFunction.InsertString(2, _T("傅里叶(蛮力法)"));
	mSRFunction.InsertString(3, _T("傅里叶(快速)"));
	mSRFunction.SetCurSel(0);
	return 0;
}

void CTabScaleRotateDlg::DoProcess(CImage * img)
{
	switch (mSRFunction.GetCurSel())
	{
	case 0: // Scale
		this->DoScale(img);
		break;
	case 1: // Rotate
		this->DoRotate(img);
		break;
	case 2: // Fourier
		this->DoFourier(img);
		break;
	case 3: // Fast Fourier
		this->DoFFT(img);
		break;
	default:;
	}
}

void CTabScaleRotateDlg::DoScale(CImage * img)
{
	auto *sp = new Algo::ScaleParams;
	auto th = DA->GetThreadOption();
	CString strScaleFactor, outMsg;
	mSRParameter.GetWindowTextW(strScaleFactor);
	strScaleFactor.Trim();
	sp->scale = _ttof(strScaleFactor);
	sp->src = img;
	DA->OutputLine(_T("开始处理 缩放 比例参数 ") + strScaleFactor);
	CImage *dest = new CImage;
	*(DA->DlgImage) = dest; // Change main dialog `img` to new one
	// The original CImage will be deleted in the callback of thread callback
	dest->Create(
		static_cast<int>(img->GetWidth() * sp->scale + 0.5),
		static_cast<int>(img->GetHeight() * sp->scale + 0.5),
		img->GetBPP()
	);
	auto tasks = Algo::SplitTask(dest, th.type == ThreadOption::OpenCL ? 1 : th.count);
	if (th.type == ThreadOption::Afx)
	{
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = sp;
			AfxBeginThread(Algo::ImageScale, tasks + i);
		}
	}
	else if (th.type == ThreadOption::OpenMP)
	{
#pragma omp parallel for num_threads(th.count)
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = sp;
			Algo::ImageScale(tasks + i);
		}
	}
	else
	{
		tasks[0].ctx = sp;
		tasks[0].wParam = 0;
		AfxBeginThread(Algo::ImageScaleCL, tasks);
	}
}

inline static Algo::vec2<double> Vec2AfterRotate(const Algo::vec2<double> &p, const double sa, const double ca)
{
	Algo::vec2<double> ret;
	ret.x =  p.x * ca + p.y * sa;
	ret.y = -p.x * sa + p.y * ca;
	return ret;
}

void CTabScaleRotateDlg::DoRotate(CImage * img)
{
	auto *rp = new Algo::RotateParams;
	auto th = DA->GetThreadOption();
	CString strRotateAngle, outMsg;
	mSRParameter.GetWindowTextW(strRotateAngle);
	strRotateAngle.Trim();
	rp->angle = _ttof(strRotateAngle) * acos(-1) / 180.0; // to radian
	rp->src = img;
	DA->OutputLine(_T("开始处理 旋转 角度(°) ") + strRotateAngle);
	CImage *dest = new CImage;
	*(DA->DlgImage) = dest; // Change main dialog `img` to new one
	// The original CImage will be deleted in the callback of thread callback

	// 计算旋转后的坐标
	using Algo::vec2;
	double sina = sin(rp->angle);
	double cosa = cos(rp->angle);
	vec2<double> lefttop = Vec2AfterRotate({ 0.0, 0.0 }, sina, cosa);
	vec2<double> leftbottom = Vec2AfterRotate({ 0.0, (double)img->GetHeight() - 1.0 }, sina, cosa);
	vec2<double> righttop = Vec2AfterRotate({ (double)img->GetWidth() - 1.0, 0.0 }, sina, cosa);
	vec2<double> rightbottom = Vec2AfterRotate({ (double)img->GetWidth() - 1.0, (double)img->GetHeight() - 1.0 }, sina, cosa);
	
	CString out;
	out.Format(_T("rad=%lf, cos(angle)=%lf, sin(angle)=%lf"), rp->angle, cosa, sina);
	DA->OutputLine(out);

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
	double left   = MIN(lefttop.x, MIN(righttop.x, MIN(leftbottom.x, rightbottom.x)));
	double right  = MAX(lefttop.x, MAX(righttop.x, MAX(leftbottom.x, rightbottom.x)));
	double top    = MIN(lefttop.y, MIN(righttop.y, MIN(leftbottom.y, rightbottom.y)));
	double bottom = MAX(lefttop.y, MAX(righttop.y, MAX(leftbottom.y, rightbottom.y)));
#undef MIN
#undef MAX

	int width  = (int)abs(right - left) + 1;
	int height = (int)abs(top - bottom) + 1;
	dest->Create(width, height, img->GetBPP());
	out.Format(_T("New dimension: width=%d, height=%d"), width, height);
	DA->OutputLine(out);

	auto tasks = Algo::SplitTask(dest, th.count);
	if (th.type == ThreadOption::Afx)
	{
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = rp;
			AfxBeginThread(Algo::ImageRotate, tasks + i);
		}
	}
	else if (th.type == ThreadOption::OpenMP)
	{
#pragma omp parallel for num_threads(th.count)
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = rp;
			Algo::ImageRotate(tasks + i);
		}
	}
	else
	{
		tasks[0].ctx = rp;
		tasks[0].wParam = 0;
		AfxBeginThread(Algo::ImageRotateCL, tasks);
	}
}

void CTabScaleRotateDlg::DoFourier(CImage * img)
{
	AfxMessageBox(_T("蛮力计算傅里叶变换的复杂度高，请耐心等待!"));
	auto th = DA->GetThreadOption();
	DA->OutputLine(_T("开始处理 傅里叶变换"));
	CImage *grayscale = new CImage;
	grayscale->Create(img->GetWidth(), img->GetHeight(), img->GetBPP());
	*(DA->DlgImage) = grayscale;
	auto tasks = Algo::SplitTask(grayscale, th.count);
	if (th.type == ThreadOption::Afx)
	{
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = img;
			AfxBeginThread(Algo::FourierTransform, tasks + i);
		}
	}
	else if(th.type == ThreadOption::OpenMP)
	{
#pragma omp parallel for num_threads(th.count)
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = img;
			Algo::FourierTransform(tasks + i);
		}
	}
	else
	{
		tasks[0].ctx = img;
		tasks[0].wParam = 0;
		AfxBeginThread(Algo::FourierTransformCL, tasks);
	}
}

static bool IsPowerOf2(int x)
{
	int cnt = 0;
	for (int i = 0; i < 32; ++i)
	{
		if ((x & 1) == 1)
			++cnt;
		if (cnt > 1)
			return false;
		x >>= 1;
	}
	return true;
}

static int CeilToPowerOf2(int x)
{
	int mask = 1 << 30;
	for (int i = 0; i < 31; ++i)
	{
		if ((mask & x) != 0)
			return mask << 1;
		mask >>= 1;
	}
	return 0;
}

void CTabScaleRotateDlg::DoFFT(CImage * img)
{
	DA->ThreadSlider->SetPos(1); // single thread FFT
	auto th = DA->GetThreadOption();
	DA->OutputLine(_T("开始处理 快速傅里叶变换"));
	// 缩放到2的整数次方
	if (!IsPowerOf2(img->GetWidth()) || !IsPowerOf2(img->GetHeight()))
	{
		int newwidth = CeilToPowerOf2(img->GetWidth());
		int newheight = CeilToPowerOf2(img->GetHeight());
		CString o;
		o.Format(_T("Scaling to %d×%d (single thread, blocked)"), newwidth, newheight);
		DA->OutputLine(o);
		auto newimg = new CImage;
		newimg->Create(newwidth, newheight, img->GetBPP());
		*(DA->DlgImage) = newimg;
		auto sp = new Algo::ScaleParams;
		sp->src = img;
		auto task = Algo::SplitTask(newimg, 1);
		task->ctx = sp;
		task->wParam = 0; // implicit single thread
		Algo::ImageScale(task);
		// done
		img = newimg;
	}
	DA->OutputLine(_T("Doing FFT"));
	CImage *grayscale = new CImage;
	grayscale->Create(img->GetWidth(), img->GetHeight(), img->GetBPP());
	*(DA->DlgImage) = grayscale;
	auto tasks = Algo::SplitTask(grayscale, th.count);
	if (th.type == ThreadOption::Afx)
	{
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = img;
			tasks[i].wParam = 0;
			AfxBeginThread(Algo::FastFourierTransform, tasks + i);
		}
	}
	else
	{
#pragma omp parallel for num_threads(th.count)
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = img;
			tasks[i].wParam = 0;
			Algo::FastFourierTransform(tasks + i);
		}
	}
}

BEGIN_MESSAGE_MAP(CTabScaleRotateDlg, CDialogEx)
END_MESSAGE_MAP()


// CTabScaleRotateDlg 消息处理程序
