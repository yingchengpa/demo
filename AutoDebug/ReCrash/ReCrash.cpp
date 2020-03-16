// ReCrash.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <tlhelp32.h> // 声明快照函数的头文件 
#include <string>

std::string GetProNameFromID(int nProceID)
{

	std::string strProcName;

	PROCESSENTRY32 pe32;  
    // 在使用这个结构之前，先设置它的大小  
    pe32.dwSize = sizeof(pe32);   
      
    // 给系统内的所有进程拍一个快照  
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
    if(hProcessSnap == INVALID_HANDLE_VALUE)  
    {  
        printf(" CreateToolhelp32Snapshot调用失败！ \n");  
        return "";  
    }  
      
    // 遍历进程快照，轮流显示每个进程的信息  
    BOOL bMore = ::Process32First(hProcessSnap, &pe32);  
    while(bMore)  
    {  
        //printf(" 进程名称：%s \n", pe32.szExeFile);  
        //printf(" 进程ID号：%u \n\n", pe32.th32ProcessID); 
		if(pe32.th32ProcessID == nProceID)
		{
			strProcName = pe32.szExeFile;
			break;
		}
  
        bMore = ::Process32Next(hProcessSnap, &pe32);  
    }  
  
    // 不要忘记清除掉snapshot对象  
    ::CloseHandle(hProcessSnap);  

	return strProcName;
}

std::string GetRootPath()
{
	char moduleName[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, moduleName, MAX_PATH);

	std::string strPath(moduleName);
	strPath = strPath.substr(0, strPath.find_last_of(('\\')) + 1);

	return strPath;
}

std::string string_format(const char * _Format, ...)
{
#define DESCRIB_TEMP 512
    char szMsg[DESCRIB_TEMP] = { 0 };

    va_list marker = NULL;
    va_start(marker, _Format);

    (void)vsprintf(szMsg, _Format, marker);

    va_end(marker);

    return szMsg;
} 

/* 使用方法，把程序和ntsd放置在同一个目录即可*/

int main(int argc, char* argv[])
{
	int nProcessID = 0;//-p 
	int nParam1  = 0;//-c 

	/*
	for(int i = 0 ;i < argc;i++)
	{
		OutputDebugString(argv[i]);
		OutputDebugString("\n");
	}*/

	if(argc > 2)
	{

		//-p 123 -c 768
		nProcessID = atoi(argv[2]);
		nParam1 = atoi(argv[4]);

		std::string strNtsd =  string_format("%sntsd.exe",GetRootPath().c_str());
		std::string strNtCmd = string_format(".dump /f /u %s%s.dmp ;q ",GetRootPath().c_str(),GetProNameFromID(nProcessID).c_str());

		std::string strCMD = string_format("\"%s\" -p %ld -e %ld -g -c \"%s\"",strNtsd.c_str(),nProcessID,nParam1,strNtCmd.c_str());

		OutputDebugString(strCMD.c_str());

		WinExec(strCMD.c_str(), SW_HIDE);
	}
	else
	{
	}

	return 0;
}

