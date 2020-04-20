#pragma once

/*
 _tmain 异常捕获用法


LONG WINAPI ExpFilter(struct _EXCEPTION_POINTERS *pExp)
{
	UwlWriteMiniDMP(pExp,g_sCfgFile);
	
	//使用tcdingding钉钉告警，及时知道服务器异常
	//std::string strErr = std::string("警报发生: xx程序发生崩溃\n") ;
	//g_dingding.Init("https://oapi.dingtalk.com/robot/send?access_token=");
	//g_dingding.SendMsg(AnsiToUtf8Str(strErr));
	return EXCEPTION_EXECUTE_HANDLER;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	
	//设置异常处理函数,非win10 系统可以捕获crt的栈溢出异常
	UwlSetUnhandledExceptionFilter(ExpFilter);

	.....

	return nRetCode;
*/

//************************************
// Method:    UwlWriteMiniDMP
// FullName:  UwlWriteMiniDMP
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: struct _EXCEPTION_POINTERS * pExp
// Parameter: CString strIni  服务的配置文件, 
// Remarks: 根据strIni中的[Debug] dumpType 类型生成minidump or fulldump
// 	dumpType=1 ，fulldump   --- 不需要配置默认，dump文件大
//  dumpType=0 ，minidump   

//  当d:\盘 空间不足600MB时，dump不再写入
//************************************
void UwlWriteMiniDMP(struct _EXCEPTION_POINTERS* pExp, CString strIni = "");

//************************************
// Method:    UwlSetUnhandledExceptionFilter
// FullName:  UwlSetUnhandledExceptionFilter
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
// Remark: 设置异常捕获处理函数
// step1 : SetUnhandledExceptionFilter
// step2 : if not win10 system : DisableSetUnhandledExceptionFilter()
//************************************
void UwlSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);


//************************************
// Method:    UwlGetOSName
// FullName:  UwlGetOSName
// Access:    public 
// Returns:   CString
// Qualifier: 
// Remarks: 返回操作系统描述，分为："2000","XP","2003","Vista","7","8","8.1","10","unknow"
//************************************
CString UwlGetOSName();

//************************************
// Method:    UwlGetDiskFreeSapce
// FullName:  UwlGetDiskFreeSapce
// Access:    public 
// Returns:   double
// Qualifier:
// Parameter: CString strDiskName
// Remarks: 返回指定盘符的剩余空间,单位（MB）	，失败返回 -1.0
//************************************
//double UwlGetDiskFreeSapce(CString strDiskName);

