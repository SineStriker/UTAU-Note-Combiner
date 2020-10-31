
// SimpleFCDlg.h: 头文件
//

#pragma once


// CSimpleFCDlg 对话框
class CSimpleFCDlg : public CDialog
{
// 构造
public:
	CSimpleFCDlg(CWnd* pParent = nullptr);	// 标准构造函数

	CSimpleFCDlg(const double &num, CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLEFC_DIALOG };
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

	double currentLength;
public:
	afx_msg void OnBnClickedOk();

	void setCurrentLength(const double &num);
	double getCurrentLength();
	CString getLengthStr();

	CComboBox Combo1;
};
