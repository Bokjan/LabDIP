#pragma once

// TabDialogExamples �Ի���

class TabDialogExamples : public CDialog
{
	DECLARE_DYNAMIC(TabDialogExamples)

public:
	TabDialogExamples(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~TabDialogExamples();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TABDIALOGEXAMPLES };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CComboBox mComboFunction;
public:
	void DoProcess(CImage *image);
};
