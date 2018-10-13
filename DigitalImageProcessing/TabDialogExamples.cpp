// TabDialogExamples.cpp : 实现文件
//

#include "stdafx.h"
#include "DigitalImageProcessing.h"
#include "TabDialogExamples.h"
#include "afxdialogex.h"


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
}


BEGIN_MESSAGE_MAP(TabDialogExamples, CDialog)
END_MESSAGE_MAP()


// TabDialogExamples 消息处理程序
