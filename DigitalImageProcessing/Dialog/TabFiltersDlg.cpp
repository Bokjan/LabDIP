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
	mFunction.InsertString(1, _T("平滑线性滤波"));
	mFunction.InsertString(2, _T("高斯滤波"));
	mFunction.InsertString(3, _T("维纳滤波"));
	mFunction.SetCurSel(0);
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
		this->DoGaussNoise(image);
		break;
	default:;
	}
}

void CTabFiltersDlg::DoGaussNoise(CImage * img)
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
	/*CImage *grayscale = new CImage;
	grayscale->Create(img->GetWidth(), img->GetHeight(), img->GetBPP());
	*(DA->DlgImage) = grayscale;
	auto tasks = Algo::SplitTask(grayscale, th.count);
	if (th.type == ThreadOption::Afx)
	{
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = img;
			AfxBeginThread(Algo::ImageFourierTransform, tasks + i);
		}
	}
	else
	{
#pragma omp parallel for num_threads(th.count)
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = img;
			AfxBeginThread(Algo::ImageFourierTransform, tasks + i);
		}
	}*/
}


BEGIN_MESSAGE_MAP(CTabFiltersDlg, CDialog)
END_MESSAGE_MAP()


// CTabFiltersDlg 消息处理程序
