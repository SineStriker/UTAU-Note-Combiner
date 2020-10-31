
// SimpleFC.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "SimpleFC.h"
#include "SimpleFCDlg.h"

#include "Combiner.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSimpleFCApp

BEGIN_MESSAGE_MAP(CSimpleFCApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSimpleFCApp 构造

CSimpleFCApp::CSimpleFCApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CSimpleFCApp 对象

CSimpleFCApp theApp;

Combiner theCombiner;


// CSimpleFCApp 初始化

BOOL CSimpleFCApp::InitInstance()
{
	LANGID lid = GetUserDefaultLangID();

	bool isChi = lid == 0X0804;

	CString cmd = ::GetCommandLine();

	int argc = 0;
	LPWSTR *argv = ::CommandLineToArgvW(cmd, &argc);

	TCHAR lpszTemp[512];

	// 获取临时文件夹路径
	GetTempPath(512, lpszTemp);

	CString strTempPath;
	CString strTempFile;
	CString str = _T("50");
	CFile file;
	double num;

	strTempPath.Format(_T("%s\\UTAU_Note_Combiner_MFC"), lpszTemp);
	strTempFile = strTempPath + _T("\\data.txt");

	// 保存默认滑音长度的值
	if (!PathFileExists(strTempPath))
	{
		CreateDirectory(strTempPath, NULL);
	};
	
	if (!PathFileExists(strTempFile)) {
		fstream fs;
		string line;

		line = CT2A(str.GetString());

		fs.open(strTempFile, ios::out);
		fs << line;
		fs.close();

		num = _wtof(str);
	}
	else {
		fstream fs(strTempFile,ios::in);
		string line;
		getline(fs, line);
		fs.close();

		str = CA2T(line.c_str());
		num = _wtof(str);
	}

	if (argc > 1) {

		// 获取第一个参数
		CString path = argv[1];

		string s;

		s = CT2A(path.GetString());

		theCombiner.setDefaultPitch(num/2);
		theCombiner.loadFile(s);
		theCombiner.Main();

		CString crossWarning;
		CString modeWarning;
		CString tempoWarning;
		CString notesWarning;
		CString onlyWarning;
		CString title;

		BOOL crossNameValid;
		BOOL modeNameValid;
		BOOL tempoNameValid;
		BOOL notesNameValid;
		BOOL onlyNameValid;
		BOOL titleValid;

		if (isChi) {

			crossNameValid = crossWarning.LoadStringW(IDS_MSGCROSS);
			modeNameValid = modeWarning.LoadStringW(IDS_MSGMODE);
			tempoNameValid = tempoWarning.LoadStringW(IDS_MSGTEMPO);
			notesNameValid = notesWarning.LoadStringW(IDS_MSGNOTES);
			onlyNameValid = onlyWarning.LoadStringW(IDS_MSGONLY);
			titleValid = title.LoadStringW(IDS_MSGTITLE);
		}
		else {

			crossNameValid = crossWarning.LoadStringW(IDS_MSGCROSS_EN);
			modeNameValid = modeWarning.LoadStringW(IDS_MSGMODE_EN);
			tempoNameValid = tempoWarning.LoadStringW(IDS_MSGTEMPO_EN);
			notesNameValid = notesWarning.LoadStringW(IDS_MSGNOTES_EN);
			onlyNameValid = onlyWarning.LoadStringW(IDS_MSGONLY_EN);
			titleValid = title.LoadStringW(IDS_MSGTITLE_EN);
		}

		ASSERT(crossNameValid && modeNameValid && tempoNameValid && notesNameValid && onlyNameValid && titleValid);

		switch (theCombiner.getCode()) {

		case 1:
			// 一切正常
			theCombiner.Output(s);
			return false;
			break;

		case 3:

			// 只有一个音符
			if (MessageBox(NULL, onlyWarning, title, MB_OKCANCEL | MB_ICONINFORMATION | MB_DEFBUTTON1) == IDOK)
			{
				theCombiner.Output(s);
			}
			return false;
			break;

		case 2:

			// 音高曲线交叉
			if (MessageBox(NULL, crossWarning, title, MB_OKCANCEL | MB_ICONINFORMATION | MB_DEFBUTTON2) == IDOK)
			{
				theCombiner.Output(s);
			}
			return false;
			break;

		case 10:

			// Mode 2 未打开
			MessageBox(NULL, modeWarning, title, MB_ICONEXCLAMATION);
			return false;
			break;

		case 11:
			// 选区内曲速变化
			MessageBox(NULL, tempoWarning, title, MB_ICONEXCLAMATION);
			return false;
			break;

		case 13:

			// 没有选择音符，则当作没有命令行参数传递的情况
			//MessageBox(NULL, notesWarning, title, MB_ICONEXCLAMATION);
			//return false;
			break;

		default:
			return false;
			break;
		}

	}
	else {

	}

	// 如果获取到命令行参数，那么以下过程全部跳过。

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();


	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CSimpleFCDlg dlg(num);
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码

		// 退出时保存更改的默认滑音长度

		str = dlg.getLengthStr();

		fstream fs;
		string line;

		line = CT2A(str.GetString());

		fs.open(strTempFile, ios::out);
		fs << line;
		fs.close();
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
	}


#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

