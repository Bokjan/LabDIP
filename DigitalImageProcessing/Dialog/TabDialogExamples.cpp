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
	auto th = DA->GetThreadOption();
	auto tasks = Algo::SplitTask(image, th.count);
	switch (mComboFunction.GetCurSel())
	{
	case 0: // salt & pepper noise
		DA->OutputLine(_T("��ʼ���� ��������"));
		for (int i = 0; i < th.count; ++i)
			AfxBeginThread(Algo::SaltAndPepperNoise, tasks + i);
		break;
	case 1: // median filter
		break;
	}
}
