
// loadDllDlg.h: 头文件
//

#pragma once


// CloadDllDlg 对话框
class CloadDllDlg : public CDialogEx
{
// 构造
public:
	CloadDllDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOADDLL_DIALOG };
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
	afx_msg void OnBnClickedButton1();
	CString m_strTraget;

private:
	BOOL LoadDll(DWORD pId);
	void UnloadDll(DWORD pId);
	BOOL IsloadedDll(DWORD dwProcessid);
	DWORD FindNotLoadedDllPid();
	DWORD FindExePid();
};
