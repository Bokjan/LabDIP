
// DigitalImageProcessingDlg.cpp : 实现文件
//

#include "../stdafx.h"
#include "../DigitalImageProcessing.h"
#include "DigitalImageProcessingDlg.h"
#include "../Util/DisplayAgent.h"
#include "../Algo/General.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDigitalImageProcessingDlg 对话框

CDigitalImageProcessingDlg::CDigitalImageProcessingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIGITALIMAGEPROCESSING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDigitalImageProcessingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, mPictureControl);
	DDX_Control(pDX, IDC_OUTPUTAREA, mOutput);
	DDX_Control(pDX, IDC_TAB_OPERATIONS, mTabOps);
	DDX_Control(pDX, IDC_PICTURE_RIGHT, mPictureControlRight);
	DDX_Control(pDX, IDC_COMBO_THREAD_TYPE, mComboThreadType);
	DDX_Control(pDX, IDC_SLIDER_THREAD, mSliderThreadNum);
	DDX_Control(pDX, IDC_STATIC_THREAD_NUMBER, mTextThreadNum);
	DDX_Control(pDX, IDC_CHECK_HUNDRED, mCheckBoxHundred);
	DDX_Control(pDX, IDC_BUTTON_EXECUTE, mButtonExecute);
	DDX_Control(pDX, IDC_CHECK_ORIGINAL, mCheckUseOriginal);
}

BEGIN_MESSAGE_MAP(CDigitalImageProcessingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CDigitalImageProcessingDlg::OnBnClickedButtonOpen)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_OPERATIONS, &CDigitalImageProcessingDlg::OnTcnSelchangeTabOperations)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_THREAD, &CDigitalImageProcessingDlg::OnNMCustomdrawSlider)
	ON_BN_CLICKED(IDC_BUTTON_EXECUTE, &CDigitalImageProcessingDlg::OnBnClickedButtonExecute)
	ON_MESSAGE(WM_USER_EXECUTE_FINISHED, &CDigitalImageProcessingDlg::OnExecuteFinished)
	ON_BN_CLICKED(IDC_BUTTON_CLEARDBG, &CDigitalImageProcessingDlg::OnBnClickedButtonCleardbg)
END_MESSAGE_MAP()


// CDigitalImageProcessingDlg 消息处理程序

BOOL CDigitalImageProcessingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	img = nullptr;
	this->InitDisplayAgent();
	DA->OutputLine(_T("Initializing..."));
	this->SetTabOperations();
	this->InitThreadWidgets();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDigitalImageProcessingDlg::InitDisplayAgent(void)
{
	auto da = DisplayAgent::GetInstance();
	da->HWnd = this->GetSafeHwnd();
	da->OutputArea = &this->mOutput;
	da->PictureLeft = &this->mPictureControl;
	da->PictureRight = &this->mPictureControlRight;
	da->ThreadType = &this->mComboThreadType;
	da->ThreadSlider = &this->mSliderThreadNum;
	da->DlgImage = &this->img;
}

void CDigitalImageProcessingDlg::InitThreadWidgets(void)
{
	mComboThreadType.InsertString(0, _T("WinAFX"));
	mComboThreadType.InsertString(1, _T("OpenMP"));
	// Test whether OpenCL directory exist
	auto dwAttrib = GetFileAttributes(_T(REPO_ROOT"OpenCL"));
	if (INVALID_FILE_ATTRIBUTES != dwAttrib && 0 != (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
	{
		mComboThreadType.InsertString(2, _T("OpenCL"));
	}
	else
	{
		DA->OutputLine(_T("OpenCL files not exist! Disabling OpenCL."));
	}
	mComboThreadType.SetCurSel(0); // Windows
	// Set correct range: get core count
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	mSliderThreadNum.SetRange(1, si.dwNumberOfProcessors, TRUE);
	mSliderThreadNum.SetPos(si.dwNumberOfProcessors);
}

void CDigitalImageProcessingDlg::SetTabOperations(void)
{
	// https://blog.csdn.net/csdn1507/article/details/78486603
	mTabOps.InsertItem(0, _T("演示功能"));
	mTabOps.InsertItem(1, _T("插值与傅里叶"));
	mTabOps.InsertItem(2, _T("噪声与滤波"));

	Tab1.Create(IDD_TAB_EXAMPLES_DIALOG, &mTabOps);
	CRect rs;
	mTabOps.GetClientRect(&rs);
	// HiDPI 支持，需要修改rs.top的偏移量，100%为22。
	rs.top += 44;
	Tab1.MoveWindow(&rs);
	Tab1.ShowWindow(true);
	mTabOps.SetCurSel(0);

	Tab2.Create(IDD_TAB_SCALEROTATE_DIALOG, &mTabOps);
	Tab2.MoveWindow(&rs);

	Tab3.Create(IDD_TAB_FILTERS_DIALOG, &mTabOps);
	Tab3.MoveWindow(&rs);
}

void CDigitalImageProcessingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDigitalImageProcessingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CDigitalImageProcessingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDigitalImageProcessingDlg::OnBnClickedButtonOpen()
{
	TCHAR szFilter[] = _T("JPEG(*jpg)|*.jpg|*.bmp|TIFF(*.tif)|*.tif|All Files (*.*)|*.*||");
	CString filePath("");
	CFileDialog fileOpenDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (fileOpenDialog.DoModal() == IDOK)
	{
		filePath = fileOpenDialog.GetPathName();
	}
	else
	{
		return;
	}
	strFilePath = filePath;
	if (img != nullptr)
		delete img;
	img = new CImage;
	img->Load(strFilePath);
	DA->PaintCImageToCStatic(img, &mPictureControl);
	DA->OutputLine(_T("Loaded image: ") + strFilePath);
}

void CDigitalImageProcessingDlg::OnTcnSelchangeTabOperations(NMHDR *pNMHDR, LRESULT *pResult)
{
	Tab1.ShowWindow(false);
	Tab2.ShowWindow(false);
	Tab3.ShowWindow(false);
	int selection = mTabOps.GetCurSel();
	switch (selection)
	{
	case 0:
		Tab1.ShowWindow(true);
		break;
	case 1:
		Tab2.ShowWindow(true);
		break;
	case 2:
		Tab3.ShowWindow(true);
		break;
	default:;
	}
	*pResult = 0;
}


void CDigitalImageProcessingDlg::OnNMCustomdrawSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	CString text;
	text.Format(_T("%d"), mSliderThreadNum.GetPos());
	mTextThreadNum.SetWindowTextW(text);
	*pResult = 0;
}

LRESULT CDigitalImageProcessingDlg::OnExecuteFinished(WPARAM wParam, LPARAM lParam)
{
	static int cnt = 0;
	++cnt;
	if (DisplayAgent::GetInstance()->GetThreadOption().count == cnt || wParam == 0)
	{
		cnt = 0;
		auto p = (ParallelParams*)lParam;
		if (p->cb != nullptr)
			p->cb(p);
		DA->PrintTimeElapsed();
		DA->PaintCImageToCStatic(((ParallelParams*)lParam)->img, &mPictureControlRight);
		if (p->ctx != nullptr)
			delete p->ctx;
		if (p->thctx != nullptr)
			delete[] p->thctx; // 目前thctx是ParallelParams[]
	}
	this->mButtonExecute.EnableWindow(true);
	return LRESULT();
}

void CDigitalImageProcessingDlg::OnBnClickedButtonExecute()
{
	if (this->img == nullptr)
	{
		DA->OutputLine(_T("No image loaded"));
		return;
	}
	this->mButtonExecute.EnableWindow(false);
	if (mCheckUseOriginal.GetCheck())
	{
		delete img;
		img = new CImage;
		img->Load(this->strFilePath);
	}
	DA->StartTick();
	switch (mTabOps.GetCurFocus())
	{
	case 0: // Example operations in tutorial
		Tab1.DoProcess(img);
		break;
	case 1: // Scale, rotate, and Fourier transformation
		Tab2.DoProcess(img);
		break;
	case 2: // Gauss noise, and filters
		Tab3.DoProcess(img);
		break;
	default:;
	}
}

void CDigitalImageProcessingDlg::OnBnClickedButtonCleardbg()
{
	DA->ClearOutput();
}
