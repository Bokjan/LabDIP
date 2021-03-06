
// DigitalImageProcessingDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "TabExamplesDlg.h"
#include "TabScaleRotateDlg.h"
#include "TabFiltersDlg.h"

// CDigitalImageProcessingDlg 对话框
class CDigitalImageProcessingDlg : public CDialogEx
{
// 构造
public:
	CDigitalImageProcessingDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIGITALIMAGEPROCESSING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CImage *img;
	CString strFilePath;
	afx_msg void OnBnClickedButtonOpen();
	CStatic mPictureControl;
	CStatic mPictureControlRight;
	CEdit mOutput;
	CTabCtrl mTabOps;
	CTabExamplesDlg Tab1;
	CTabScaleRotateDlg Tab2;
	CTabFiltersDlg Tab3;
	afx_msg void OnTcnSelchangeTabOperations(NMHDR *pNMHDR, LRESULT *pResult);
private:
	void SetTabOperations(void);
	void InitDisplayAgent(void);
	void InitThreadWidgets(void);
public:
	CComboBox mComboThreadType;
	CSliderCtrl mSliderThreadNum;
	CStatic mTextThreadNum;
	CButton mCheckBoxHundred;
	CButton mButtonExecute;
	afx_msg void OnNMCustomdrawSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonExecute();
	CButton mCheckUseOriginal;
	afx_msg LRESULT OnExecuteFinished(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonCleardbg();
};
