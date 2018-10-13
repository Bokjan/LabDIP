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

	DECLARE_MESSAGE_MAP()
};
