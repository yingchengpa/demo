#pragma once


// CLogonDlg 对话框

class CLogonDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLogonDlg)

public:
	CLogonDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CLogonDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strHost;
	CString m_strUserName;
	afx_msg void OnBnClickedOk();
};
