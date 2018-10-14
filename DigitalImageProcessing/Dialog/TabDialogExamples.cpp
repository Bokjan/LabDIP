// TabDialogExamples.cpp : ʵ���ļ�
//

#include "../stdafx.h"
#include "../DigitalImageProcessing.h"
#include "TabDialogExamples.h"
#include "../Util/DisplayAgent.h"
#include "../Algo/General.h"

// TabDialogExamples �Ի���

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


// TabDialogExamples ��Ϣ�������
BOOL TabDialogExamples::OnInitDialog()
{
	CDialog::OnInitDialog();
	mComboFunction.AddString(_T("��������"));
	mComboFunction.AddString(_T("��ֵ�˲�"));
	mComboFunction.SetCurSel(0);
	return TRUE;
}

void TabDialogExamples::DoProcess(CImage *image)
{
	auto th = DisplayAgent::GetInstance()->GetThreadOption();
	switch (mComboFunction.GetCurSel())
	{
	case 0: // noise
		ParallelParams p;
		p.begin = 0;
		p.end = image->GetWidth() * image->GetHeight();
		p.img = image;
		Algo::SaltAndPepperNoise(&p);
		break;
	case 1: // median filter
		break;
	}
}