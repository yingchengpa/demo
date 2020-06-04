
// loadDllDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "loadDll.h"
#include "loadDllDlg.h"
#include "afxdialogex.h"

#include <direct.h>
#include <stdlib.h>
#include <TlHelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DLL_NAME "xxxx.dll"

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


// CloadDllDlg 对话框



CloadDllDlg::CloadDllDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOADDLL_DIALOG, pParent)
	, m_strTraget(_T("hooktest.exe"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CloadDllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strTraget);
}

BEGIN_MESSAGE_MAP(CloadDllDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CloadDllDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CloadDllDlg 消息处理程序

BOOL CloadDllDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CloadDllDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CloadDllDlg::OnPaint()
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CloadDllDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CloadDllDlg::LoadDll(DWORD pId)
{
	if (0 == pId)
	{
		return FALSE;
	}

	if (IsloadedDll(pId))
	{
		return TRUE;
	}

	char* curPath = NULL;
	if ((curPath = _getcwd(NULL, 0)) == NULL)
	{
		//MessageBoxA(NULL, "获取当前工作目录失败", "错误", 0);
		return FALSE;
	}

	char dllFile[MAX_PATH] = { 0 };
	sprintf_s(dllFile, "%s\\%s", curPath, DLL_NAME);

	//打开进程
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);
	if (hProcess == NULL)
	{
		MessageBoxA("进程打开失败", "错误", 0);
		return FALSE;
	}

	//在微信进程中申请内存
	LPVOID pAddress = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pAddress == NULL)
	{
		MessageBoxA("内存分配失败", "错误", 0);
		return FALSE;
	}

	//写入dll路径到微信进程
	//DWORD dwWrite = 0;
	ULONG_PTR dwWrite = 0;
	if (WriteProcessMemory(hProcess, pAddress, dllFile, MAX_PATH, &dwWrite) == 0)
	{
		MessageBoxA("路径写入失败", "错误", 0);
		return FALSE;
	}

	//获取LoadLibraryA函数地址
	FARPROC pLoadLibraryAddress = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (pLoadLibraryAddress == NULL)
	{
		MessageBoxA("获取LoadLibraryA函数地址失败", "错误", 0);
		return FALSE;
	}

	//远程线程注入dll
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryAddress, pAddress, 0, NULL);
	if (hRemoteThread == NULL)
	{
		MessageBoxA("远程线程注入失败", "错误", 0);
		return FALSE;
	}

	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);

	//MessageBoxA(NULL, "dll加载成功", "Tip", 0);
	return TRUE;
}


void CloadDllDlg::UnloadDll(DWORD pId)
{
	if (pId == 0)
	{
		return;
	}

	//遍历模块
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pId);
	MODULEENTRY32 ME32 = { 0 };
	ME32.dwSize = sizeof(MODULEENTRY32);
	BOOL isNext = Module32First(hSnap, &ME32);
	BOOL flag = FALSE;
	while (isNext)
	{
		USES_CONVERSION;
		if (strcmp((ME32.szModule), DLL_NAME) == 0)
		{
			flag = TRUE;
			break;
		}
		isNext = Module32Next(hSnap, &ME32);
	}
	if (flag == FALSE)
	{
		MessageBoxA("找不到目标模块", "错误", 0);
		return;
	}

	//打开目标进程
	HANDLE hPro = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);
	//获取FreeLibrary函数地址
	FARPROC pFun = GetProcAddress(GetModuleHandleA("kernel32.dll"), "FreeLibrary");

	//创建远程线程执行FreeLibrary
	HANDLE hThread = CreateRemoteThread(hPro, NULL, 0, (LPTHREAD_START_ROUTINE)pFun, ME32.modBaseAddr, 0, NULL);
	if (!hThread)
	{
		MessageBoxA("创建远程线程失败", "错误", 0);
		return;
	}

	CloseHandle(hSnap);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hPro);
	MessageBox("dll卸载成功", "Tip", 0);
}


BOOL CloadDllDlg::IsloadedDll(DWORD dwProcessid)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	//初始化模块信息结构体
	MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };
	//创建模块快照 1 快照类型 2 进程ID
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessid);
	//如果句柄无效就返回false
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		MessageBoxA("创建模块快照失败", "错误", MB_OK);
		return FALSE;
	}

	//通过模块快照句柄获取第一个模块的信息
	if (!Module32First(hModuleSnap, &me32))
	{
		MessageBoxA("获取第一个模块的信息失败", "错误", MB_OK);
		//获取失败则关闭句柄
		CloseHandle(hModuleSnap);
		return FALSE;
	}

	do
	{
		if (StrCmp(me32.szModule, TEXT(DLL_NAME)) == 0)
		{
			return TRUE;
		}

	} while (Module32Next(hModuleSnap, &me32));

	return FALSE;
}

DWORD CloadDllDlg::FindNotLoadedDllPid()
{
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(hProcess, &pe32) == TRUE)
	{
		do
		{
			USES_CONVERSION;
			if (strcmp(m_strTraget.GetBuffer(0), (pe32.szExeFile)) == 0)
			{
				if (!IsloadedDll(pe32.th32ProcessID))
				{
					return pe32.th32ProcessID;
				}
			}
		} while (Process32Next(hProcess, &pe32));
	}

	return 0;
}

DWORD CloadDllDlg::FindExePid()
{
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(hProcess, &pe32) == TRUE)
	{
		do
		{
			USES_CONVERSION;
			if (strcmp(m_strTraget.GetBuffer(0), (pe32.szExeFile)) == 0)
			{
				return pe32.th32ProcessID;
			}
		} while (Process32Next(hProcess, &pe32));
	}

	return 0;
}

bool isLoad = false;
DWORD pid = 0;
void CloadDllDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if (!isLoad)
	{
		pid = FindExePid();
		if (pid == 0)
		{
			MessageBox("程序未运行");
			isLoad = !isLoad;

			return;

		}
		if (LoadDll(pid))
		{
			isLoad = true;

			GetDlgItem(IDC_BUTTON1)->SetWindowText("脱离");
		}
	}
	else
	{
		UnloadDll(pid);

		isLoad = false;
		GetDlgItem(IDC_BUTTON1)->SetWindowText("附加");
	}
}
