#pragma once

// TabDialogExamples 对话框

class TabDialogExamples : public CDialog
{
	DECLARE_DYNAMIC(TabDialogExamples)

public:
	TabDialogExamples(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~TabDialogExamples();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TABDIALOGEXAMPLES };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	const static int MaxThread = 4;
	CComboBox mComboFunction;
	CComboBox mComboThread;
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnNMCustomdrawSliderThreadnum(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl mSlider;
	CStatic mThreadNumText;
};
