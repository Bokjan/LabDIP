#pragma once


// CTabScaleRotateDlg 对话框

class CTabScaleRotateDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTabScaleRotateDlg)

public:
	CTabScaleRotateDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTabScaleRotateDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_SCALEROTATE_DIALOG };
#endif

private:
	void DoScale(CImage *img);
	void DoRotate(CImage *img);
	void DoFourier(CImage *img);
	void DoFFT(CImage *img);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox mSRFunction;
	CEdit mSRParameter;

	void DoProcess(CImage *img);
};
