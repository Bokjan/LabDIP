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
	mSRFunction.InsertString(2, _T("傅里叶"));
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
	auto tasks = Algo::SplitTask(dest, th.count);
	if (th.type == ThreadOption::Afx)
	{
		for (int i = 0; i < th.count; ++i)
		{
			tasks[i].ctx = sp;
			AfxBeginThread(Algo::ImageScale, tasks + i);
		}
	}
	else 
	{
		AfxMessageBox(_T("Not implemented yet (OpenMP)"));
	}
}


BEGIN_MESSAGE_MAP(CTabScaleRotateDlg, CDialogEx)
END_MESSAGE_MAP()


// CTabScaleRotateDlg 消息处理程序
