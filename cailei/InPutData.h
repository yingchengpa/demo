#pragma once


#include <string>
// CInPutData 对话框

class CInData
{
public:
	std::string    m_strOrder;
	std::string    m_strRemark;
};

class CInPutData : public CDialogEx
{
	DECLARE_DYNAMIC(CInPutData)

public:
	CInPutData(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CInPutData();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strDate;
	afx_msg void OnBnClickedButton1();
};
