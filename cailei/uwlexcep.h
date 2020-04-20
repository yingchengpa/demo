#pragma once

/*
 _tmain �쳣�����÷�


LONG WINAPI ExpFilter(struct _EXCEPTION_POINTERS *pExp)
{
	UwlWriteMiniDMP(pExp,g_sCfgFile);
	
	//ʹ��tcdingding�����澯����ʱ֪���������쳣
	//std::string strErr = std::string("��������: xx����������\n") ;
	//g_dingding.Init("https://oapi.dingtalk.com/robot/send?access_token=");
	//g_dingding.SendMsg(AnsiToUtf8Str(strErr));
	return EXCEPTION_EXECUTE_HANDLER;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	
	//�����쳣������,��win10 ϵͳ���Բ���crt��ջ����쳣
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
// Parameter: CString strIni  ����������ļ�, 
// Remarks: ����strIni�е�[Debug] dumpType ��������minidump or fulldump
// 	dumpType=1 ��fulldump   --- ����Ҫ����Ĭ�ϣ�dump�ļ���
//  dumpType=0 ��minidump   

//  ��d:\�� �ռ䲻��600MBʱ��dump����д��
//************************************
void UwlWriteMiniDMP(struct _EXCEPTION_POINTERS* pExp, CString strIni = "");

//************************************
// Method:    UwlSetUnhandledExceptionFilter
// FullName:  UwlSetUnhandledExceptionFilter
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
// Remark: �����쳣��������
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
// Remarks: ���ز���ϵͳ��������Ϊ��"2000","XP","2003","Vista","7","8","8.1","10","unknow"
//************************************
CString UwlGetOSName();

//************************************
// Method:    UwlGetDiskFreeSapce
// FullName:  UwlGetDiskFreeSapce
// Access:    public 
// Returns:   double
// Qualifier:
// Parameter: CString strDiskName
// Remarks: ����ָ���̷���ʣ��ռ�,��λ��MB��	��ʧ�ܷ��� -1.0
//************************************
//double UwlGetDiskFreeSapce(CString strDiskName);

