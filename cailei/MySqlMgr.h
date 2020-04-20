#pragma once
#include <string>
#include <list>

class CData
{
public:
	std::string strOrder;
	std::string strRemark;
	BOOL        bCancell = 0;
	std::string strName;     // 操作员
	std::string strOpTime;	 // 录入时间
	std::string strInTime; 	 // 入库时间
};


bool EasyLogon(std::string strHost, std::string username, std::string pwd);

bool GetDataByTime(std::list<CData> &oData,std::string strDateTime);

bool GetDataOverTimeAndCancell(std::list<CData>& oData, std::string strDateTime,int nCancell = -1);

bool GetDataByTimeAndCancell(std::list<CData>& oData, std::string strDateTime,int nCancall);

bool UpdateOrderCancell(std::string strOrder, std::string strName,std::string strTime,int nCancall);

bool GetDataByOrder(std::list<CData>& oData, std::string strOrder);

//插入一条数据
bool InsertData(const std::string &strOrder,const std::string &strRemark);

//定时保活
bool EasyKeepAlive();
