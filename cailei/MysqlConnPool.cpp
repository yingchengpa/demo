
#include "MysqlConnPool.h"

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

CMysqlConnPool::CMysqlConnPool()
	:m_bInitSucc(false)
	, m_pDriver(nullptr)
	, m_nMaxPoolSize(0)
	, m_nCurrentPollSize(0)
{
}


CMysqlConnPool::~CMysqlConnPool()
{
	UnInit();
}

int CMysqlConnPool::Init(const std::string &sHostName, const std::string &sUserName, const std::string &sPwd, const std::string &sDBName, int nPoolSize)
{
	std::lock_guard<std::mutex> oLock(m_oConnListMutex);
	if (true == m_bInitSucc)
	{
		LOG_ERROR(_T("Re-initialization."));
		return ERR_FAILD;
	}

	m_nMaxPoolSize = nPoolSize;
	m_sHostName = sHostName;
	m_sUserName = sUserName;
	m_sDBName = sDBName;
	m_sPwd = sPwd;

	try
	{
		m_pDriver = get_driver_instance();
		if (nullptr == m_pDriver)
		{
			LOG_ERROR(_T("get_driver_instance return null."));
			return ERR_FAILD;
		}

		for (int i = 0; i < nPoolSize; ++i)
		{
			sql::Connection *pConn = Connect();
			if (nullptr == pConn)
			{
				LOG_ERROR(_T("Connect faild."));
				continue;
			}

			pConn->setSchema(m_sDBName);
			m_oFreeConnList.push_back(pConn);
			++m_nCurrentPollSize;
		}
	}
	catch (sql::SQLException &e)
	{
		LOG_SQLEXCEPTION;
		return ERR_FAILD;
	}
	
	if (m_nCurrentPollSize <= 0)
	{
		return ERR_FAILD;
	}

	m_bInitSucc = true;
	return ERR_SUCC;
}

void CMysqlConnPool::UnInit()
{
	std::lock_guard<std::mutex> oLock(m_oConnListMutex);
	if (false == m_bInitSucc)
	{
		return;
	}

	// 正常情况下运行此函数m_oUsingConnList应该已经为空，即使不为空也不释放其资源，避免内存冲突
	try
	{
		for (auto itr = m_oFreeConnList.begin(); itr != m_oFreeConnList.end(); ++itr)
		{
			SAFE_DELETE(*itr);
		}
	}
	catch (sql::SQLException &e)
	{
		LOG_SQLEXCEPTION;
	}
	m_oFreeConnList.clear();

	m_sHostName.clear();
	m_sUserName.clear();
	m_sPwd.clear();
	m_sDBName.clear();

	m_nMaxPoolSize = 0;
	m_nCurrentPollSize = 0;
	m_pDriver = nullptr;
	m_bInitSucc = false;
}

sql::Connection * CMysqlConnPool::Connect()
{
	if (nullptr == m_pDriver)
	{
		return nullptr;
	}

	try
	{
		return m_pDriver->connect(m_sHostName, m_sUserName, m_sPwd);
	}
	catch (sql::SQLException &e)
	{
		LOG_SQLEXCEPTION;
		return nullptr;
	}
}

bool CMysqlConnPool::ReConnect(sql::Connection *pConn)
{
	if (nullptr == pConn)
	{
		return false;
	}

	try
	{
		bool bRet = pConn->reconnect();
		pConn->setSchema(m_sDBName);

		return bRet;
	}
	catch (sql::SQLException &e)
	{
		LOG_SQLEXCEPTION;
		return false;
	}
}

sql::Connection * CMysqlConnPool::GetConnection()
{
	std::lock_guard<std::mutex> oLock(m_oConnListMutex);
	if (m_oFreeConnList.size() > 0)
	{
		sql::Connection *pConn = m_oFreeConnList.front();
		m_oFreeConnList.pop_front();
		m_oUsingConnList.push_back(pConn);
		return pConn;
	}

	if (m_nCurrentPollSize >= m_nMaxPoolSize)
	{
		LOG_WARN(_T("Conntions busy."));
		return nullptr;
	}

	sql::Connection *pConn = Connect();
	if (nullptr == pConn)
	{
		LOG_WARN(_T("Connect faild."));
		return nullptr;
	}

	try
	{
		pConn->setSchema(m_sDBName);
	}
	catch (sql::SQLException &e)
	{
		LOG_SQLEXCEPTION;
		SAFE_DELETE(pConn);
		return nullptr;
	}

	++m_nCurrentPollSize;
	m_oUsingConnList.push_back(pConn);
	return pConn;
}

void CMysqlConnPool::GiveBackConnection(sql::Connection *pConn, bool bBad /*= false*/)
{
	if (nullptr == pConn)
	{
		return;
	}

	if (false == EraseConnFromUsinglist(pConn))
	{
		return;
	}

	do
	{
		if (false == bBad)
		{
			break;
		}

		if (true == pConn->isValid())
		{
			break;
		}

		bool bSuc = ReConnect(pConn);
		if (true == bSuc)
		{
			break;
		}

		// 重连失败则销毁该连接，下次获取连接的时候会尝试建新连接
		SAFE_DELETE(pConn);
		--m_nCurrentPollSize;

		return;

	} while (0);

	AddConnToFreelist(pConn);
}

bool CMysqlConnPool::EraseConnFromUsinglist(const sql::Connection *pConn)
{
	std::lock_guard<std::mutex> oLock(m_oConnListMutex);
	auto itr = std::find(m_oUsingConnList.begin(), m_oUsingConnList.end(), pConn);
	if (itr == m_oUsingConnList.end())
	{
		return false;
	}

	m_oUsingConnList.erase(itr);
	return true;
}

void CMysqlConnPool::AddConnToFreelist(sql::Connection *pConn)
{
	std::lock_guard<std::mutex> oLock(m_oConnListMutex);
	m_oFreeConnList.push_front(pConn);
}
