#include "pch.h"
#include "MySqlMgr.h"
#include "MysqlConnPool.h"

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

// 获取并检查connector
#define GET_AND_CHECK_CONN	sql::Connection *pConn = nullptr;\
	CAutoConnection oAuto(g_oConnPool, pConn);\
	if (nullptr == pConn)\
	{\
		LOG_ERROR(_T("null connection."));\
		return false;\
	}

// 宽字符转UTF8 
std::string UwlUnicodeToUtf8Str(const std::wstring & in)
{
	std::string s(in.length() * 3 + 1, ' ');
	size_t len = ::WideCharToMultiByte(CP_UTF8, 0, in.c_str(), in.length(), &s[0], s.length(), NULL, NULL);
	s.resize(len);
	return s;
}

// 宽字符转ansi
std::string UwlUnicodeToAnsiStr(const std::wstring & in)
{
	std::string s(in.length() * 2 + 1, ' ');
	size_t len = ::WideCharToMultiByte(CP_ACP, 0, in.c_str(), in.length(), &s[0], s.length(), NULL, NULL);
	s.resize(len);
	return s;
}

// ansi 转宽字符 
std::wstring UwlAnsiToUnicodeWStr(const std::string & in)
{
	std::wstring s(in.length(), _T(' '));
	size_t len = ::MultiByteToWideChar(CP_ACP, 0, in.c_str(), in.length(), &s[0], s.length());
	s.resize(len);
	return s;
}
// UTF8 转宽字符 
std::wstring UwlUtf8ToUnicodeWStr(const std::string & in)
{
	std::wstring s(in.length(), _T(' '));
	size_t len = ::MultiByteToWideChar(CP_UTF8, 0, in.c_str(), in.length(), &s[0], s.length());
	s.resize(len);
	return s;
}

//ansi 转utf8
std::string UwlAnsiToUtf8Str(const std::string & in)
{
	std::wstring wstr = UwlAnsiToUnicodeWStr(in);

	return UwlUnicodeToUtf8Str(wstr);
}

//utf8 转ansi
std::string UwlUtf8ToAnsiStr(const std::string & in)
{
	std::wstring wstr = UwlUtf8ToUnicodeWStr(in);

	return UwlUnicodeToAnsiStr(wstr);
}

std::string string_format(const char* _Format, ...)
{
#define DESCRIB_TEMP 512
	char szMsg[DESCRIB_TEMP] = { 0 };

	va_list marker = NULL;
	va_start(marker, _Format);

	(void)vsprintf_s(szMsg, DESCRIB_TEMP - 1, _Format, marker);

	va_end(marker);

	return szMsg;
}

CMysqlConnPool g_oConnPool;

std::string g_strDBName = "gameresultdb";
std::string g_strTblName = "test";

bool EasyLogon(std::string strHost,std::string username,std::string pwd)
{
	std::string strUrl = string_format("tcp://%s:3306", strHost.c_str());
	int nRet = g_oConnPool.Init(strUrl, "cailei", "123qweasd", g_strDBName, 4);
	if (ERR_SUCC != nRet)
	{
		LOG_ERROR(_T("ConnPool init faild."));
		return false;
	}
	return true;
}

bool EasyKeepAlive()
{
	GET_AND_CHECK_CONN;

	const sql::SQLString s_sql = "select now() as Systemtime";
	try
	{
		std::unique_ptr<sql::Statement> pStmt(pConn->createStatement());
		int nCount = pStmt->executeUpdate(s_sql);
	}
	catch (sql::SQLException& e)
	{
		oAuto.SetConnBad();
		LOG_SQLEXCEPTION;
		return false;
	}
	return true;
}

bool GetDataBySql(std::string strSql, std::list<CData>& oList)
{
	GET_AND_CHECK_CONN;

	const sql::SQLString s_sql = strSql;

	try
	{
		std::unique_ptr<sql::Statement> pStmt(pConn->createStatement());
		std::unique_ptr<sql::ResultSet> pRes(pStmt->executeQuery(s_sql));

		// 正序
		while (pRes->next())
		{
			CData oItem;
			oItem.strOrder = UwlUtf8ToAnsiStr(pRes->getString(1));
			oItem.strRemark = UwlUtf8ToAnsiStr(pRes->getString(2));
			oItem.bCancell = pRes->getInt(3);
			oItem.strName = UwlUtf8ToAnsiStr(pRes->getString(4));
			oItem.strOpTime = UwlUtf8ToAnsiStr(pRes->getString(5));
			oItem.strInTime = UwlUtf8ToAnsiStr(pRes->getString(6));

			oList.push_back(oItem);
		}
	}
	catch (sql::SQLException& e)
	{
		oAuto.SetConnBad();
		LOG_SQLEXCEPTION;

		return false;
	}
	return true;
}

bool GetDataByTime(std::list<CData>& oData, std::string strDateTime)
{
	std::string  strSql = string_format("select * from %s where intime = '%s' ", g_strTblName.c_str(), strDateTime.c_str());

	return GetDataBySql(strSql,oData);
}

bool GetDataOverTimeAndCancell(std::list<CData>& oData, std::string strDateTime, int nCancell )
{
	std::string  strSql;

	if(nCancell == -1)
	{
		strSql = string_format("select * from %s where intime >= '%s' ", g_strTblName.c_str(), strDateTime.c_str());
	}
	else
	{
		strSql = string_format("select * from %s where intime >= '%s' and cancell = %d ", g_strTblName.c_str(), strDateTime.c_str(),nCancell);
	}

	return GetDataBySql(strSql, oData);
}

bool GetDataByTimeAndCancell(std::list<CData>& oData, std::string strDateTime, int nCancall)
{
	
	std::string  strSql = string_format("select * from %s where intime = '%s' and cancell = %d ", g_strTblName.c_str(), strDateTime.c_str(),nCancall);

	return GetDataBySql(strSql, oData);
}

bool UpdateOrderCancell(std::string strOrder, std::string strName,std::string strTime, int nCancall)
{
	GET_AND_CHECK_CONN;

	std::string  strSql = string_format("update %s set cancell = %d ,name = '%s', timestamp = '%s' where ordernum = '%s' ", g_strTblName.c_str(),  nCancall, strName.c_str(),strTime.c_str(),strOrder.c_str());

	const sql::SQLString s_sql = strSql;

	try
	{
		std::unique_ptr<sql::Statement> pStmt(pConn->createStatement());
		int nCount = pStmt->executeUpdate(UwlAnsiToUtf8Str(s_sql));
	}
	catch (sql::SQLException& e)
	{
		oAuto.SetConnBad();
		LOG_SQLEXCEPTION;
		return false;
	}
	return true;
}

bool GetDataByOrder(std::list<CData>& oData, std::string strOrder)
{
	std::string strSql = string_format("select * from %s where ordernum LIKE ", g_strTblName.c_str());

	strSql += "'%" + strOrder + "%'";
	return GetDataBySql(strSql, oData);
}

// 插入数据/更新同理
bool TestInsert()
{
	GET_AND_CHECK_CONN;

	const sql::SQLString s_sql = "INSERT INTO test (ordernum, remark, cancell,name,timestamp,intime) VALUES('yykdjkd', '裤袜*1 衣服*2', 0,'蔡lei','2018-09-13 21:15:10','2018-09-13')";

	try
	{
		std::unique_ptr<sql::Statement> pStmt(pConn->createStatement());
		int nCount = pStmt->executeUpdate(UwlAnsiToUtf8Str(s_sql));
	}
	catch (sql::SQLException& e)
	{
		oAuto.SetConnBad();
		LOG_SQLEXCEPTION;
		return false;
	}
	return true;
}

bool InsertData(const std::string& strOrder, const std::string& strRemark)
{
	CTime time = CTime::GetCurrentTime();

	std::string strTime = string_format("%04d-%02d-%02d", time.GetYear(), time.GetMonth(), time.GetDay());

	std::string strSql = string_format("INSERT INTO test(ordernum,remark,cancell,intime) values ('%s','%s',0,'%s')",(strOrder).c_str() , (strRemark).c_str(), strTime.c_str());
	GET_AND_CHECK_CONN;

	const sql::SQLString s_sql = strSql;

	try
	{
		std::unique_ptr<sql::Statement> pStmt(pConn->createStatement());
		int nCount = pStmt->executeUpdate(UwlAnsiToUtf8Str(s_sql));
	}
	catch (sql::SQLException& e)
	{
		oAuto.SetConnBad();
		LOG_SQLEXCEPTION;
		return false;
	}
	return true;
}

// 查询
bool TestSelect()
{
	GET_AND_CHECK_CONN;

	const sql::SQLString s_sql = "SELECT * FROM tbltest";

	try
	{
		std::unique_ptr<sql::Statement> pStmt(pConn->createStatement());
		std::unique_ptr<sql::ResultSet> pRes(pStmt->executeQuery(s_sql));

		// 正序
		while (pRes->next())
		{
			LOG_INFO(_T("ID:%d, Name:%s, Time:%s"), pRes->getInt(1), pRes->getString(2).c_str(), pRes->getString(3).c_str());
		}
		// 倒序
		pRes->afterLast();
		while (pRes->previous())
		{
			LOG_INFO(_T("ID:%d, Name:%s, Time:%s"), pRes->getInt(1), pRes->getString(2).c_str(), pRes->getString(3).c_str());
		}
	}
	catch (sql::SQLException& e)
	{
		oAuto.SetConnBad();
		LOG_SQLEXCEPTION;
		return false;
	}
	return true;
}
