#pragma once

// CTabExamplesDlg 对话框

class CTabExamplesDlg : public CDialog
{
	DECLARE_DYNAMIC(CTabExamplesDlg)

public:
	CTabExamplesDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTabExamplesDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_EXAMPLES_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CComboBox mComboFunction;
public:
	void DoProcess(CImage *image);
};
