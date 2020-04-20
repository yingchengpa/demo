// uwl9.0 (Universal Windows Library)
//
// Copyright(c) Xiaoyao Network Co., Ltd. All rights reserved.

// uwlexcep.cpp :异常捕获函数实现模块
//

#include "pch.h"
#include  <dbghelp.h > 
#pragma comment(lib,  "dbghelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//返回盘符的剩余空间,单位（MB）
double UwlGetDiskFreeSapce(CString strDiskName)
{
	double dfre = -1.0;// 默认小于0

	DWORD64 qwFreeBytes, qwFreeBytesToCaller, qwTotalBytes;
	BOOL  bResult = GetDiskFreeSpaceEx(strDiskName.GetBuffer(0),
		(PULARGE_INTEGER)&qwFreeBytesToCaller,
		(PULARGE_INTEGER)&qwTotalBytes,
		(PULARGE_INTEGER)&qwFreeBytes);
	if (bResult)
	{
		dfre = qwFreeBytesToCaller / (1024.0 * 1024.0);
	}
	return dfre;
}

CString UwlGetOSName()
{
	CString osName = ("");
	int i = 0, j = 0;
	_asm
	{
		pushad
		mov ebx, fs: [0x18] ; get self pointer from TEB
		mov eax, fs: [0x30] ; get pointer to PEB / database
		mov ebx, [eax + 0A8h]; get OSMinorVersion
		mov eax, [eax + 0A4h]; get OSMajorVersion
		mov j, ebx
		mov i, eax
		popad
	}

	if ((i == 5) && (j == 0))
	{
		osName = ("2000");
	}
	else if ((i == 5) && (j == 1))
	{
		osName = ("XP");
	}
	else if ((i == 5) && (j == 2))
	{
		osName = ("2003");
	}
	else if ((i == 6) && (j == 0))
	{
		osName = ("Vista");
	}
	else if ((i == 6) && (j == 1))
	{
		osName = ("7");
	}
	else if ((i == 6) && (j == 2))
	{
		osName = ("8");
	}
	else if ((i == 6) && (j == 3))
	{
		osName = ("8.1");
	}
	else if ((i == 10) && (j == 0))
	{
		osName = ("10");
	}
	else
	{
		osName = ("unknow");
	}
	return osName;
}

void UwlWriteMiniDMP(struct _EXCEPTION_POINTERS* pExp, CString strIni)
{
	static bool Writedumpflag = false;  // 保证同时只允许一个线程在写dump
	if (Writedumpflag)
	{
		return;
	}

	//硬盘空间 <600.0M 时，不允许记录dump，以免将硬盘写满
	if (UwlGetDiskFreeSapce("D:") < 600.0)
	{
		return;
	}

	Writedumpflag = true;

	BOOL bFullDump = TRUE;
	// get exefile fullpath
	if (strIni.GetLength() != 0)
	{
		bFullDump = GetPrivateProfileInt(_T("Debug"), _T("dumpType"), 1, strIni.GetBuffer(0));//可以通过配置文件来修改保存的方式
	}

	CString   strDumpFile;
	TCHAR szFilePath[MAX_PATH];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	*strrchr(szFilePath, '\\') = 0;

	SYSTEMTIME stTime;
	GetLocalTime(&stTime);

	if (0 == bFullDump)
	{
		strDumpFile.Format("%s\\crash_m_[%02d-%02d %02d-%02d-%02d].dmp", szFilePath, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		HANDLE   hFile = CreateFile(strDumpFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION   ExInfo;
			ExInfo.ThreadId = ::GetCurrentThreadId();
			ExInfo.ExceptionPointers = pExp;
			ExInfo.ClientPointers = NULL;
			//   write   the   dump 
			(void)MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
			CloseHandle(hFile);
		}
	}
	else
	{
		strDumpFile.Format("%s\\crash_f_[%02d-%02d %02d-%02d-%02d].dmp", szFilePath, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		HANDLE   hFile = CreateFile(strDumpFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION   ExInfo;
			ExInfo.ThreadId = ::GetCurrentThreadId();
			ExInfo.ExceptionPointers = pExp;
			ExInfo.ClientPointers = NULL;
			//   write   the   dump 
			(void)MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, (MINIDUMP_TYPE)0x9b67, &ExInfo, NULL, NULL);
			CloseHandle(hFile);
		}
	}
	Writedumpflag = false;
}

#ifndef _M_IX86

#error "The following code only works for x86!"

#endif

/************************************************************************/
/* 之所以应用程序捕获不到栈溢出异常，原因是因为新版本的CRT实现在异常处理中强制删除所有应用程序先前设置的捕获函数，如下所示：

__crtUnhandledException()
{
SetUnhandledExceptionFilter(NULL);

UnhandledExceptionFilter(&ExceptionPointers);
}
解决方法是拦截CRT调用SetUnhandledExceptionFilter函数，使之无效。在X86平台下，可以使用以下代码                                                                     * /
/************************************************************************/

void UwlDisableSetUnhandledExceptionFilter()
{
	void* addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")), "SetUnhandledExceptionFilter");
	if (addr)
	{
		unsigned char code[16];
		int size = 0;

		code[size++] = 0x33;
		code[size++] = 0xC0;
		code[size++] = 0xC2;
		code[size++] = 0x04;
		code[size++] = 0x00;
		DWORD dwOldFlag, dwTempFlag;

		(void)VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
		(void)WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
		(void)VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
	}
}

void UwlSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	::SetUnhandledExceptionFilter(lpTopLevelExceptionFilter);

	//捕获crt的栈溢出异常
	//win10 系统无法设置，会出现崩溃 。

	if ("10" != UwlGetOSName())
	{
		UwlDisableSetUnhandledExceptionFilter();
	}
}