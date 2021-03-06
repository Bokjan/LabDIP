// TabFiltersDlg.cpp: 实现文件
//

#include "../stdafx.h"
#include "../DigitalImageProcessing.h"
#include "TabFiltersDlg.h"
#include "../Algo/General.h"
#include "../Util/DisplayAgent.h"


// CTabFiltersDlg 对话框

IMPLEMENT_DYNAMIC(CTabFiltersDlg, CDialog)

CTabFiltersDlg::CTabFiltersDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TAB_FILTERS_DIALOG, pParent)
{

}

CTabFiltersDlg::~CTabFiltersDlg()
{
}

BOOL CTabFiltersDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	mFunction.InsertString(0, _T("高斯噪声"));
	mFunction.InsertString(1, _T("算术均值滤波"));
	mFunction.InsertString(2, _T("高斯滤波"));
	mFunction.InsertString(3, _T("维纳滤波"));
	mFunction.SetCurSel(0);
	mParam1.SetWindowTextW(_T("0"));
	return 0;
}

void CTabFiltersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FFUNCTION, mFunction);
	DDX_Control(pDX, IDC_EDIT_FPARAM1, mParam1);
	DDX_Control(pDX, IDC_EDIT_FPARAM2, mParam2);
}

void CTabFiltersDlg::DoProcess(CImage * image)
{
	switch (mFunction.GetCurSel())
	{
	case 0: // Gauss noise
		this->DoGaussianNoise(image);
		break;
	case 1: // Arithmetic mean filter
		this->DoArithMeanFilter(image);
		break;
	case 2: // Gaussian filter
		this->DoGaussianFilter(image);
		break;
	case 3: // Wiener filter
		this->DoWienerFiler(image);
		break;
	default:;
	}
}

void CTabFiltersDlg::DoGaussianNoise(CImage * img)
{
	CString s;
	double mean, stddev;
	mParam1.GetWindowTextW(s);
	mean = _ttof(s);
	mParam2.GetWindowTextW(s);
	stddev = _ttof(s);
	s.Format(_T("开始处理 高斯噪声 均值=%f 标准差=%f"), mean, stddev);
	DA->OutputLine(s);
	auto th = DA->GetThreadOption();
	auto ctx = new Algo::GaussianParams;
	ctx->src = nullptr;
	ctx->mean = mean;
	ctx->stddev = stddev;
	auto tasks = Algo::SplitTask(img, th.count);
	if (th.type == ThreadOption::Afx)
	{
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = ctx;
			AfxBeginThread(Algo::GaussianNoise, tasks + i);
		}
	}
	else if (th.type == ThreadOption::OpenMP)
	{
#pragma omp parallel for num_threads(th.count)
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = ctx;
			Algo::GaussianNoise(tasks + i);
		}
	}
	else
	{
		tasks[0].ctx = ctx;
		tasks[0].wParam = 0;
		AfxBeginThread(Algo::GaussianNoiseCL, tasks);
	}
}

void CTabFiltersDlg::DoGaussianFilter(CImage * img)
{
	CString s;
	double mean, stddev;
	mParam1.GetWindowTextW(s);
	mean = _ttof(s);
	mParam2.GetWindowTextW(s);
	stddev = _ttof(s);
	s.Format(_T("开始处理 高斯滤波 均值=%f 标准差=%f 模板大小3*3"), mean, stddev);
	DA->OutputLine(s);
	auto th = DA->GetThreadOption();
	auto ctx = new Algo::GaussianParams;
	ctx->src = img;
	ctx->mean = mean;
	ctx->stddev = stddev;
	auto writebuf = new CImage;
	writebuf->Create(img->GetWidth(), img->GetHeight(), img->GetBPP());
	*(DA->DlgImage) = writebuf;
	auto tasks = Algo::SplitTask(writebuf, th.count);
	if (th.type == ThreadOption::Afx)
	{
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = ctx;
			AfxBeginThread(Algo::GaussianFilter, tasks + i);
		}
	}
	else if (th.type == ThreadOption::OpenMP)
	{
#pragma omp parallel for num_threads(th.count)
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = ctx;
			Algo::GaussianFilter(tasks + i);
		}
	}
	else
	{
		tasks[0].ctx = ctx;
		tasks[0].wParam = 0;
		AfxBeginThread(Algo::GaussianFilterCL, tasks);
	}
}

void CTabFiltersDlg::DoArithMeanFilter(CImage * img)
{
	DA->OutputLine(_T("开始处理 算术均值滤波 模板大小3*3"));
	auto th = DA->GetThreadOption();
	auto writebuf = new CImage;
	writebuf->Create(img->GetWidth(), img->GetHeight(), img->GetBPP());
	*(DA->DlgImage) = writebuf;
	auto tasks = Algo::SplitTask(writebuf, th.count);
	if (th.type == ThreadOption::Afx)
	{
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = img;
			AfxBeginThread(Algo::ArithMeanFilter, tasks + i);
		}
	}
	else
	{
#pragma omp parallel for num_threads(th.count)
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = img;
			Algo::ArithMeanFilter(tasks + i);
		}
	}
}

void CTabFiltersDlg::DoWienerFiler(CImage * img)
{
	DA->OutputLine(_T("开始处理 维纳滤波 3*3"));
	auto th = DA->GetThreadOption();
	auto writebuf = new CImage;
	writebuf->Create(img->GetWidth(), img->GetHeight(), img->GetBPP());
	*(DA->DlgImage) = writebuf;
	auto tasks = Algo::SplitTask(writebuf, th.count);
	if (th.type == ThreadOption::Afx)
	{
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = img;
			AfxBeginThread(Algo::WienerFilter, tasks + i);
		}
	}
	else
	{
#pragma omp parallel for num_threads(th.count)
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = img;
			Algo::WienerFilter(tasks + i);
		}
	}

}


BEGIN_MESSAGE_MAP(CTabFiltersDlg, CDialog)
END_MESSAGE_MAP()


// CTabFiltersDlg 消息处理程序
