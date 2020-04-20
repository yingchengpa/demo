// InPutData.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication1.h"
#include "InPutData.h"
#include "afxdialogex.h"
#include "MySqlMgr.h"
#include <string>
#include <list>


// CInPutData 对话框

IMPLEMENT_DYNAMIC(CInPutData, CDialogEx)

CInPutData::CInPutData(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
	, m_strDate(_T(""))
{

}

CInPutData::~CInPutData()
{
}

void CInPutData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strDate);
	DDX_Control(pDX, IDC_BUTTON1, m_bEnter);
}


BEGIN_MESSAGE_MAP(CInPutData, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CInPutData::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, &CInPutData::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CInPutData 消息处理程序

void ParseStringToData(const CString &strData,std::list<CInData> &oList)
{
	// 分割成多行
	CString strEditStr;//编辑框的内容
	CString strTemp;//用于分割的临时字符串
	strEditStr = strData.GetString();

	std::list<std::string> oListLine;
	int nPos = strData.Find(_T("\r\n"));
	strTemp = _T("");
	while (0 <= nPos)
	{
		strTemp = strEditStr.Left(nPos);
		if (!strTemp.IsEmpty())
		{
			oListLine.push_back(strTemp.GetBuffer(0));
		}
		strEditStr = strEditStr.Right(strEditStr.GetLength() - nPos - 2);
		nPos = strEditStr.Find(_T("\r\n"));
	}
	if (!strEditStr.IsEmpty())
	{
		oListLine.push_back(strTemp.GetBuffer(0));
	}//多行字符串分割完毕

	//多行进行解析
	for (auto& value : oListLine)
	{
		size_t npost = value.find_first_of('\t');
		std::string strOrder = value.substr(0, npost);
		std::string strRemark = value.substr(npost + 1, value.size() - nPos);

		CInData oData;
		oData.m_strOrder = strOrder;
		oData.m_strRemark = strRemark;

		oList.push_back(oData);
	}
}

void CInPutData::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	std::list<CInData> oList;
	ParseStringToData(m_strDate, oList);

	for (auto& value : oList)
	{
		if (!InsertData(value.m_strOrder, value.m_strRemark))
		{
			MessageBox("未登陆，或连接断开");
			return;
		}
	}
	MessageBox("成功导入");
}

int GetLineNum(const CString& strData)
{
	// 分割成多行
	CString strEditStr;//编辑框的内容
	CString strTemp;//用于分割的临时字符串
	strEditStr = strData.GetString();

	std::list<std::string> oListLine;
	int nPos = strData.Find(_T("\r\n"));
	strTemp = _T("");
	while (0 <= nPos)
	{
		strTemp = strEditStr.Left(nPos);
		if (!strTemp.IsEmpty())
		{
			oListLine.push_back(strTemp.GetBuffer(0));
		}
		strEditStr = strEditStr.Right(strEditStr.GetLength() - nPos - 2);
		nPos = strEditStr.Find(_T("\r\n"));
	}
	if (!strEditStr.IsEmpty())
	{
		oListLine.push_back(strTemp.GetBuffer(0));
	}//多行字符串分割完毕

	return oListLine.size();
}

void CInPutData::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	
	// 显示行数

	UpdateData(TRUE);

	int nLineNum = GetLineNum(m_strDate);
	CString str;
	str.Format("入库%d条记录",nLineNum);

	m_bEnter.SetWindowText(str);
}
