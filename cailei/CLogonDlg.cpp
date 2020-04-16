// CLogonDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "CLogonDlg.h"
#include "afxdialogex.h"

std::string getRootPath()
{
	char moduleName[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, moduleName, MAX_PATH);

	std::string strPath(moduleName);
	strPath = strPath.substr(0, strPath.find_last_of("\\/") + 1);

	return strPath;
}

std::string g_strCfg;

// CLogonDlg 对话框

IMPLEMENT_DYNAMIC(CLogonDlg, CDialogEx)

CLogonDlg::CLogonDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{
	g_strCfg = getRootPath() + "mfcapp.ini";
	{
		char szIP[64] = { 0 };
		GetPrivateProfileString(_T("svr"), _T("host"), _T(""), szIP, sizeof(szIP), g_strCfg.c_str());
		m_strHost = szIP;
	}
	{
		char szIP[64] = { 0 };
		GetPrivateProfileString(_T("svr"), _T("name"), _T(""), szIP, sizeof(szIP), g_strCfg.c_str());
		m_strUserName = szIP;
	}
}

CLogonDlg::~CLogonDlg()
{
}

void CLogonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strHost);
	DDX_Text(pDX, IDC_EDIT2, m_strUserName);
}


BEGIN_MESSAGE_MAP(CLogonDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLogonDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLogonDlg 消息处理程序



void CLogonDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);

	WritePrivateProfileString("svr","host", m_strHost.GetBuffer(0),g_strCfg.c_str());
	WritePrivateProfileString("svr","name", m_strUserName.GetBuffer(0), g_strCfg.c_str());

	CDialogEx::OnOK();
}
