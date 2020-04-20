
// MFCApplication1Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include "MySqlMgr.h"
#include "CLogonDlg.h"
#include "InPutData.h"

#include <list>
#include <string.h>

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


// CMFCApplication1Dlg 对话框



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
	, m_strOrder(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_listCtrl);
	DDX_Text(pDX, IDC_EDIT3, m_strOrder);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_timeCtrl);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_32771, &CMFCApplication1Dlg::OnLogon)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplication1Dlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication1Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication1Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCApplication1Dlg::OnBnClickedButton4)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CMFCApplication1Dlg::OnNMRClickList2)
	ON_COMMAND(ID_32774, &CMFCApplication1Dlg::OnCancellOrder)
	ON_COMMAND(ID_32772, &CMFCApplication1Dlg::OnInputData)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 消息处理程序

BOOL CMFCApplication1Dlg::OnInitDialog()
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

	TCLOG_INIT();

	CMenu menu;
	menu.LoadMenu(IDR_MENU1);  //IDR_MENU1为菜单栏ID号  
	SetMenu(&menu);

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);//选上
	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);//不选上
	((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);//不选上


	m_listCtrl.ModifyStyle(0, LVS_REPORT);               // 报表模式  
	m_listCtrl.SetExtendedStyle(m_listCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_listCtrl.InsertColumn(0, "订单号");
	m_listCtrl.InsertColumn(1, "备注");
	m_listCtrl.InsertColumn(2, "是否已退货");
	m_listCtrl.InsertColumn(3, "收货员");
	m_listCtrl.InsertColumn(4, "收货时间");
	m_listCtrl.InsertColumn(5, "入库时间");

	CRect rect;
	m_listCtrl.GetClientRect(rect); //获得当前客户区信息  
	m_listCtrl.SetColumnWidth(0, rect.Width() / 6); //设置列的宽度。  
	m_listCtrl.SetColumnWidth(1, rect.Width() / 6);
	m_listCtrl.SetColumnWidth(2, rect.Width() / 6);
	m_listCtrl.SetColumnWidth(3, rect.Width() / 6);
	m_listCtrl.SetColumnWidth(4, rect.Width() / 6);
	m_listCtrl.SetColumnWidth(5, rect.Width() / 6);

	m_timeCtrl.SetFormat(_T("ddd ',' MMM dd ',' yyyy"));

	//m_listCtrl.InsertItem(0, "2354");
	//m_listCtrl.SetItemText(0, 1, "192.168.0.2");

	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCApplication1Dlg::OnPaint()
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
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCApplication1Dlg::InsertDateToListCtrl(const std::list<CData>& oList)
{
	m_listCtrl.DeleteAllItems();

	int nIndex = 0;
	for (auto& value : oList)
	{
		m_listCtrl.InsertItem(nIndex, value.strOrder.c_str());
		m_listCtrl.SetItemText(nIndex, 1, value.strRemark.c_str());
		if (value.bCancell == 0)
		{
			m_listCtrl.SetItemText(nIndex, 2, "未退货");
		}
		else
		{
			m_listCtrl.SetItemText(nIndex, 2, "已退货");
		}			
		m_listCtrl.SetItemText(nIndex, 3, value.strName.c_str());
		m_listCtrl.SetItemText(nIndex, 4, value.strOpTime.c_str());
		m_listCtrl.SetItemText(nIndex, 5, value.strInTime.c_str());
		nIndex++;
	}
}

//logon
void CMFCApplication1Dlg::OnLogon()
{
	// TODO: 在此添加命令处理程序代码
	CLogonDlg dlg;
	if (IDOK == dlg.DoModal())
	{
		if (!EasyLogon(dlg.m_strHost.GetBuffer(0), "", ""))
		{
			MessageBox("登陆失败，可能是网络原因，或数据库服务未启动");
			return;
		}
		else
		{
			MessageBox("登陆成功");
		}

		m_strName = dlg.m_strUserName;
	}	 	
	// menu 置灰，免得重复登陆
}

// 按入库时间查询
void CMFCApplication1Dlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CTime time;
	m_timeCtrl.GetTime(time);

	CString strTime;
	strTime.Format("%04d-%02d-%02d",time.GetYear(),time.GetMonth(),time.GetDay());

	std::list<CData> oList;
	if (!GetDataByTime(oList, strTime.GetBuffer(0)))
	{
		MessageBox("未登陆，或连接断开");
		return;
	}

	InsertDateToListCtrl(oList);
}

// 按最近7天的入库时间查询
void CMFCApplication1Dlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);

	int nCancell = -1;
	if (((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck())
	{
		nCancell = 1;
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck())
	{
		nCancell = 0;
	}


	CTime time = CTime::GetCurrentTime();

	time -= CTimeSpan(7, 0, 0, 0);

	CString strTime;
	strTime.Format("%04d-%02d-%02d", time.GetYear(), time.GetMonth(), time.GetDay());

	std::list<CData> oList;
	if (!GetDataOverTimeAndCancell(oList, strTime.GetBuffer(0), nCancell))
	{
		MessageBox("未登陆，或连接断开");
		return;
	}

	InsertDateToListCtrl(oList);
}

//按钉单号查询
void CMFCApplication1Dlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	std::list<CData> oList;
	if (!GetDataByOrder(oList, m_strOrder.GetBuffer(0)))
	{
		MessageBox("未登陆，或连接断开");
		return;
	}

	InsertDateToListCtrl(oList);
}

// 最近三天数据
void CMFCApplication1Dlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);

	int nCancell = -1;
	if (((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck())
	{
		nCancell = 1;
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck())
	{
		nCancell = 0;
	}


	CTime time = CTime::GetCurrentTime();

	time -= CTimeSpan(3, 0, 0, 0);

	CString strTime;
	strTime.Format("%04d-%02d-%02d", time.GetYear(), time.GetMonth(), time.GetDay());

	std::list<CData> oList;
	if (!GetDataOverTimeAndCancell(oList, strTime.GetBuffer(0), nCancell))
	{
		MessageBox("未登陆，或连接断开");
		return;
	}

	InsertDateToListCtrl(oList);
}


void CMFCApplication1Dlg::OnNMRClickList2(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//防止在空白区点击弹出菜单
	if (m_listCtrl.GetSelectedCount() <= 0)
	{
		return;
	}
	//下面的这段代码, 不单单适应于ListCtrl
	CMenu menu;
	POINT pt = { 0 };
	GetCursorPos(&pt);//得到鼠标点击位置
	menu.LoadMenu(IDR_MENU_LIST);//菜单资源ID
	menu.GetSubMenu(0)->TrackPopupMenu(0, pt.x, pt.y, this); //m_newListCtrl是CListCtrl对象

	*pResult = 0;
}


void CMFCApplication1Dlg::OnCancellOrder()
{
	// TODO: 在此添加命令处理程序代码
	POSITION selectItemPos = m_listCtrl.GetFirstSelectedItemPosition();
	while (selectItemPos != NULL) {
		const int selectItemIndex = m_listCtrl.GetNextSelectedItem(selectItemPos);
		CString strOrder = m_listCtrl.GetItemText(selectItemIndex, 0);

		CTime time = CTime::GetCurrentTime();

		CString strTime; 
		strTime.Format("%04d-%02d-%02d %02d:%02d:%02d", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());


		if (!UpdateOrderCancell(strOrder.GetBuffer(0), m_strName.GetBuffer(0), strTime.GetBuffer(0),1))
		{
			MessageBox("未登陆，或连接断开");
		}
		else
		{
			m_listCtrl.SetItemText(selectItemIndex, 2, "已退货");
			m_listCtrl.SetItemText(selectItemIndex, 3, m_strName);
			m_listCtrl.SetItemText(selectItemIndex, 4, strTime);
		}
	}
}


void CMFCApplication1Dlg::OnInputData()
{
	// TODO: 在此添加命令处理程序代码
	CInPutData dlg;
	dlg.DoModal();
}
