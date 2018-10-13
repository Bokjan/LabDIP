// TabDialogExamples.cpp : 实现文件
//

#include "../stdafx.h"
#include "../DigitalImageProcessing.h"
#include "TabDialogExamples.h"
#include "afxdialogex.h"
#include "../Util/DisplayAgent.h"
#include <cstdio>



// TabDialogExamples 对话框

IMPLEMENT_DYNAMIC(TabDialogExamples, CDialog)

TabDialogExamples::TabDialogExamples(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TABDIALOGEXAMPLES, pParent)
{
}

TabDialogExamples::~TabDialogExamples()
{
}

void TabDialogExamples::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FUNCTION, mComboFunction);
	DDX_Control(pDX, IDC_COMBO_THREAD, mComboThread);
	DDX_Control(pDX, IDC_SLIDER_THREADNUM, mSlider);
	DDX_Control(pDX, IDC_THREADNUM_TEXT, mThreadNumText);
}


BEGIN_MESSAGE_MAP(TabDialogExamples, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_START, &TabDialogExamples::OnBnClickedButtonStart)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_THREADNUM, &TabDialogExamples::OnNMCustomdrawSliderThreadnum)
END_MESSAGE_MAP()


// TabDialogExamples 消息处理程序
BOOL TabDialogExamples::OnInitDialog()
{
	CDialog::OnInitDialog();
	mComboFunction.AddString(_T("椒盐噪声"));
	mComboFunction.AddString(_T("中值滤波"));
	mComboFunction.SetCurSel(0);
	mComboThread.AddString(_T("Win多线程"));
	mComboThread.AddString(_T("OpenMP"));
	mComboThread.AddString(_T("CUDA"));
	mComboThread.SetCurSel(0);
	mSlider.SetRange(1, MaxThread, TRUE);
	mThreadNumText.SetWindowTextW(_T("x"));
	return TRUE;
}

void TabDialogExamples::OnBnClickedButtonStart()
{
	DisplayAgent::GetInstance()->OutputArea->SetWindowTextW(_T("fuck"));
}


void TabDialogExamples::OnNMCustomdrawSliderThreadnum(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	CString text;
	text.Format(_T("%d"), mSlider.GetPos());
	mThreadNumText.SetWindowTextW(text);
	*pResult = 0;
}
