#pragma once

// CTabExamplesDlg �Ի���

class CTabExamplesDlg : public CDialog
{
	DECLARE_DYNAMIC(CTabExamplesDlg)

public:
	CTabExamplesDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTabExamplesDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_EXAMPLES_DIALOG };
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
