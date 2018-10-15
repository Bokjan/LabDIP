
// DigitalImageProcessingDlg.cpp : ʵ���ļ�
//

#include "../stdafx.h"
#include "../DigitalImageProcessing.h"
#include "DigitalImageProcessingDlg.h"
#include "../Util/DisplayAgent.h"
#include "../Algo/General.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CDigitalImageProcessingDlg �Ի���



CDigitalImageProcessingDlg::CDigitalImageProcessingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIGITALIMAGEPROCESSING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	img = nullptr;
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
END_MESSAGE_MAP()


// CDigitalImageProcessingDlg ��Ϣ�������

BOOL CDigitalImageProcessingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	this->InitDisplayAgent();
	this->SetTabOperations();
	this->InitThreadWidgets();
	DA->OutputLine(_T("Initializing..."));

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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
}

void CDigitalImageProcessingDlg::InitThreadWidgets(void)
{
	mComboThreadType.InsertString(0, _T("WinAFX"));
	mComboThreadType.InsertString(1, _T("OpenMP"));
	mComboThreadType.SetCurSel(0); // Windows
	// Set correct range: get core count
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	mSliderThreadNum.SetRange(1, si.dwNumberOfProcessors, TRUE);
}

void CDigitalImageProcessingDlg::SetTabOperations(void)
{
	// https://blog.csdn.net/csdn1507/article/details/78486603
	mTabOps.InsertItem(0, _T("��ʾ�������˲�"));
	mTabOps.InsertItem(1, _T("��ת������"));
	mTabOps.InsertItem(2, _T("��˹����"));
	mTabOps.InsertItem(3, _T("�˲�"));
	Tab1.Create(IDD_TABDIALOGEXAMPLES, &mTabOps);
	CRect rs;
	mTabOps.GetClientRect(&rs);
	// HiDPI ֧�֣���Ҫ�޸�rs.top��ƫ������100%Ϊ22��
	rs.top += 44;
	Tab1.MoveWindow(&rs);
	Tab1.ShowWindow(true);
	mTabOps.SetCurSel(0);
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDigitalImageProcessingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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
		VERIFY(filePath = fileOpenDialog.GetPathName());
	}
	strFilePath = filePath;
	if (img != nullptr)
		delete img;
	img = new CImage;
	img->Load(strFilePath);
	DA->PaintCImageToCStatic(img, &mPictureControl);
}

void CDigitalImageProcessingDlg::OnTcnSelchangeTabOperations(NMHDR *pNMHDR, LRESULT *pResult)
{
	Tab1.ShowWindow(false);
	int selection = mTabOps.GetCurSel();
	switch (selection)
	{
	case 0:
		Tab1.ShowWindow(true);
		break;
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
	if (DisplayAgent::GetInstance()->GetThreadOption().count == cnt)
	{
		cnt = 0;
		DA->PrintTimeElapsed();
		DA->PaintCImageToCStatic(((ParallelParams*)lParam)->img, &mPictureControlRight);
		delete[] ((ParallelParams*)lParam)->thctx;
	}
	return LRESULT();
}

void CDigitalImageProcessingDlg::OnBnClickedButtonExecute()
{
	if (mCheckUseOriginal.GetCheck())
	{
		delete img;
		img = new CImage;
		img->Load(this->strFilePath);
		AfxMessageBox(_T("Reloaded the original image"));
	}
	DA->StartTick();
	switch (mTabOps.GetCurFocus())
	{
	case 0: // Example operations in tutorial
		Tab1.DoProcess(img);
		break;
	default:;
	}
}
