
// SimpleFCDlg.cpp: 实现文件

#include "pch.h"
#include "framework.h"
#include "SimpleFC.h"
#include "SimpleFCDlg.h"
#include "afxdialogex.h"

#include <Winnls.h>

#pragma comment(lib,"Kernel32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

CString GetFormatStr(double dNumber, int maxPrecision, BOOL bZeroClear);

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

BOOL CAboutDlg::OnInitDialog()
{
	LANGID lid = GetUserDefaultLangID();

	bool isChi = lid == 0X0804;

	CDialog::OnInitDialog();

	CString strNameText;
	CString strRightText;
	CString strTitleText;
	CString strBtnOK;

	BOOL bNameValid1;
	BOOL bNameValid2;
	BOOL bNameValid3;
	BOOL bNameValid4;

	if (isChi) {
		bNameValid1 = strNameText.LoadString(IDS_EXENAME);
		bNameValid2 = strRightText.LoadString(IDS_COPYRIGHT);
		bNameValid3 = strTitleText.LoadString(IDS_TITLE2);
		bNameValid4 = strBtnOK.LoadString(IDS_CONFIRM);
	}
	else {
		bNameValid1 = strNameText.LoadString(IDS_EXENAME_EN);
		bNameValid2 = strRightText.LoadString(IDS_COPYRIGHT_EN);
		bNameValid3 = strTitleText.LoadString(IDS_TITLE2_EN);
		bNameValid4 = strBtnOK.LoadString(IDS_CONFIRM_EN);
	}

	ASSERT(bNameValid1 && bNameValid2 && bNameValid3 & bNameValid4);

	SetDlgItemText(IDC_EXENAME, strNameText);
	SetDlgItemText(IDC_COPYRIGHT, strRightText);
	SetDlgItemText(IDOK, strBtnOK);

	SetWindowText(strTitleText);
	
	return TRUE;
}

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSimpleFCDlg 对话框



CSimpleFCDlg::CSimpleFCDlg(CWnd* pParent /*=nullptr*/)
	: currentLength(0), CDialog(IDD_SIMPLEFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CSimpleFCDlg::CSimpleFCDlg(const double &num, CWnd* pParent)
	: currentLength(num), CDialog(IDD_SIMPLEFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO1, Combo1);
}

BEGIN_MESSAGE_MAP(CSimpleFCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CSimpleFCDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSimpleFCDlg 消息处理程序

BOOL CSimpleFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	LANGID lid = GetUserDefaultLangID();

	bool isChi = lid == 0X0804;

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;

		if (isChi)
			bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		else
			bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX_EN);

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

	CString strLengthText;
	CString strTitleText;
	CString strBtnOK;
	CString strBtnCancel;

	BOOL bNameValid1;
	BOOL bNameValid2;
	BOOL bNameValid3;
	BOOL bNameValid4;

	if (isChi) {
		bNameValid1 = strLengthText.LoadString(IDS_LENGTH);
		bNameValid2 = strTitleText.LoadString(IDS_TITLE1);
		bNameValid3 = strBtnOK.LoadString(IDS_CONFIRM);
		bNameValid4 = strBtnCancel.LoadString(IDS_CANCEL);
	}
	else {
		bNameValid1 = strLengthText.LoadString(IDS_LENGTH_EN);
		bNameValid2 = strTitleText.LoadString(IDS_TITLE1_EN);
		bNameValid3 = strBtnOK.LoadString(IDS_CONFIRM_EN);
		bNameValid4 = strBtnCancel.LoadString(IDS_CANCEL_EN);
	}

	ASSERT(bNameValid1 && bNameValid2 && bNameValid3 & bNameValid4);

	SetDlgItemText(IDC_LENGTH, strLengthText);
	SetDlgItemText(IDOK, strBtnOK);
	SetDlgItemText(IDCANCEL, strBtnCancel);

	SetWindowText(strTitleText);

	Combo1.InsertString(0, TEXT("50"));
	Combo1.InsertString(1, TEXT("80"));
	Combo1.InsertString(2, TEXT("100"));
	Combo1.InsertString(3, TEXT("160"));
	Combo1.InsertString(4, TEXT("200"));
	Combo1.InsertString(5, TEXT("300"));

	CString str;

	str = GetFormatStr(currentLength, 6, TRUE);

	Combo1.SetWindowTextW(str);
	Combo1.SetFocus();

	return FALSE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSimpleFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSimpleFCDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSimpleFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSimpleFCDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	LANGID lid = GetUserDefaultLangID();

	bool isChi = lid == 0X0804;

	CString strJudgePos;
	double value;

	CString NaN;
	CString Vor;
	CString title;

	BOOL NaNValid;
	BOOL VorValid;
	BOOL titleValid;

	if (isChi) {
		/*crossWarning = "音高曲线交叉，是否继续？";
		modeWarning = "Mode 2 未打开！";
		tempoWarning = "选区内曲速不固定！";
		notesWarning = "没有选择音符！";
		onlyWarning = "选区只有一个音符，是否清除多余控制点？";*/

		NaNValid = NaN.LoadStringW(IDS_NOTANUM);
		VorValid = Vor.LoadStringW(IDS_TOOBIG);
		titleValid = title.LoadStringW(IDS_MSGTITLE);
	}
	else {
		/*crossWarning = "Pitch curves crossed. Continue?";
		modeWarning = "Mode2 is off!";
		tempoWarning = "BPM has changed!";
		notesWarning = "No notes selected!";
		onlyWarning = "Only one point selected. Continue to remove redundant points?";*/

		NaNValid = NaN.LoadStringW(IDS_NOTANUM_EN);
		VorValid = Vor.LoadStringW(IDS_TOOBIG_EN);
		titleValid = title.LoadStringW(IDS_MSGTITLE_EN);
	}

	ASSERT(NaNValid && VorValid && titleValid);

	Combo1.GetWindowTextW(strJudgePos);

	if (strJudgePos.SpanIncluding(_T("-0123456789.")) == strJudgePos) {

		value = _wtof(strJudgePos);

		if (value > 32767 || value < 0) {

			MessageBox(Vor,title,MB_ICONEXCLAMATION);
			return;
		}

		setCurrentLength(value);
	}
	else {
		MessageBox(NaN, title, MB_ICONEXCLAMATION);
		return;
	}

	CDialog::OnOK();
}

void CSimpleFCDlg::setCurrentLength(const double &num)
{
	currentLength = num;
}

double CSimpleFCDlg::getCurrentLength()
{
	return currentLength;
}

CString CSimpleFCDlg::getLengthStr()
{
	return GetFormatStr(currentLength,6,TRUE);
}


void CAboutDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	CDialog::OnOK();
}

CString GetFormatStr(double dNumber, int maxPrecision, BOOL bZeroClear)
{
	CString strFormat;
	if (maxPrecision < 0)
	{
		return strFormat;
	}
	else if (maxPrecision > 6)
	{
		maxPrecision = 6;
	}

	strFormat.Format(_T("%%1.%dlf"), maxPrecision);
	CString strNum;
	double justifyValue = 1 / pow(10.0, maxPrecision + 2);
	strNum.Format(strFormat, dNumber + justifyValue);

	CString strResult;
	if (bZeroClear)
	{
		strResult.Format(_T("%g"), _tstof(strNum));
	}
	else
	{
		strResult = strNum;
	}

	return strResult;
}