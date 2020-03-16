// ReCrash.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <tlhelp32.h> // �������պ�����ͷ�ļ� 
#include <string>

std::string GetProNameFromID(int nProceID)
{

	std::string strProcName;

	PROCESSENTRY32 pe32;  
    // ��ʹ������ṹ֮ǰ�����������Ĵ�С  
    pe32.dwSize = sizeof(pe32);   
      
    // ��ϵͳ�ڵ����н�����һ������  
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
    if(hProcessSnap == INVALID_HANDLE_VALUE)  
    {  
        printf(" CreateToolhelp32Snapshot����ʧ�ܣ� \n");  
        return "";  
    }  
      
    // �������̿��գ�������ʾÿ�����̵���Ϣ  
    BOOL bMore = ::Process32First(hProcessSnap, &pe32);  
    while(bMore)  
    {  
        //printf(" �������ƣ�%s \n", pe32.szExeFile);  
        //printf(" ����ID�ţ�%u \n\n", pe32.th32ProcessID); 
		if(pe32.th32ProcessID == nProceID)
		{
			strProcName = pe32.szExeFile;
			break;
		}
  
        bMore = ::Process32Next(hProcessSnap, &pe32);  
    }  
  
    // ��Ҫ���������snapshot����  
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

/* ʹ�÷������ѳ����ntsd������ͬһ��Ŀ¼����*/

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

