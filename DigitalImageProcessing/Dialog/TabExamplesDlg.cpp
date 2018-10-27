// CTabExamplesDlg.cpp : 实现文件
//

#include "../stdafx.h"
#include "../DigitalImageProcessing.h"
#include "TabExamplesDlg.h"
#include "../Util/DisplayAgent.h"
#include "../Algo/General.h"

// CTabExamplesDlg 对话框

IMPLEMENT_DYNAMIC(CTabExamplesDlg, CDialog)

CTabExamplesDlg::CTabExamplesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TAB_EXAMPLES_DIALOG, pParent)
{
}

CTabExamplesDlg::~CTabExamplesDlg()
{
}

void CTabExamplesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FUNCTION, mComboFunction);
}


BEGIN_MESSAGE_MAP(CTabExamplesDlg, CDialog)
END_MESSAGE_MAP()


// CTabExamplesDlg 消息处理程序
BOOL CTabExamplesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	mComboFunction.AddString(_T("椒盐噪声"));
	mComboFunction.AddString(_T("中值滤波"));
	mComboFunction.SetCurSel(0);
	return TRUE;
}

void CTabExamplesDlg::DoProcess(CImage *image)
{
	auto th = DA->GetThreadOption();
	auto tasks = Algo::SplitTask(image, th.count);
	switch (mComboFunction.GetCurSel())
	{
	case 0: // noise
		DA->OutputLine(_T("开始处理 椒盐噪声"));
		if (th.type == ThreadOption::Afx)
		{
			for (int i = 0; i < th.count; ++i)
				AfxBeginThread(Algo::SaltAndPepperNoise, tasks + i);
		}
		else
		{
#pragma omp parallel for num_threads(th.count)
			for (int i = 0; i < th.count; ++i)
				Algo::SaltAndPepperNoise(tasks + i);
		}
		break;
	case 1: // median filter
		DA->OutputLine(_T("开始处理 中值滤波"));
		if (th.type == ThreadOption::Afx)
		{
			for (int i = 0; i < th.count; ++i)
				AfxBeginThread(Algo::MedianFilter, tasks + i);
		}
		else
		{
#pragma omp parallel for num_threads(th.count)
			for (int i = 0; i < th.count; ++i)
				Algo::MedianFilter(tasks + i);
		}
		break;
	}
}
