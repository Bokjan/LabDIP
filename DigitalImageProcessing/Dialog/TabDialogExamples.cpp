// TabDialogExamples.cpp : 实现文件
//

#include "../stdafx.h"
#include "../DigitalImageProcessing.h"
#include "TabDialogExamples.h"
#include "../Util/DisplayAgent.h"

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
}


BEGIN_MESSAGE_MAP(TabDialogExamples, CDialog)
END_MESSAGE_MAP()


// TabDialogExamples 消息处理程序
BOOL TabDialogExamples::OnInitDialog()
{
	CDialog::OnInitDialog();
	mComboFunction.AddString(_T("椒盐噪声"));
	mComboFunction.AddString(_T("中值滤波"));
	mComboFunction.SetCurSel(0);
	return TRUE;
}

void TabDialogExamples::DoProcess(CImage *image)
{
	auto th = DisplayAgent::GetInstance()->GetThreadOption();
}