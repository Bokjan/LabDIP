
// DigitalImageProcessingDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "TabDialogExamples.h"


// CDigitalImageProcessingDlg �Ի���
class CDigitalImageProcessingDlg : public CDialogEx
{
// ����
public:
	CDigitalImageProcessingDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIGITALIMAGEPROCESSING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CImage *img;
	afx_msg void OnBnClickedButtonOpen();
	CEdit mEditInfo;
	CStatic mPictureControl;
	CStatic mPictureControlRight;
	CEdit mOutput;
	CTabCtrl mTabOps;
	TabDialogExamples Tab1;
	afx_msg void OnTcnSelchangeTabOperations(NMHDR *pNMHDR, LRESULT *pResult);
private:
	void SetTabOperations(void);
	void InitDisplayAgent(void);
};
