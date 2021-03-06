#pragma once


// CTabFiltersDlg 对话框

class CTabFiltersDlg : public CDialog
{
	DECLARE_DYNAMIC(CTabFiltersDlg)

public:
	CTabFiltersDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTabFiltersDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_FILTERS_DIALOG };
#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox mFunction;
	CEdit mParam1;
	CEdit mParam2;
	void DoProcess(CImage *image);
private:
	void DoGaussianNoise(CImage *img);
	void DoGaussianFilter(CImage *img);
	void DoArithMeanFilter(CImage *img);
	void DoWienerFiler(CImage *img);
};
