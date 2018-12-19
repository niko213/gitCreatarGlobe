#pragma once

//log文件路径  
#define LOG_FILE_NAME "log.txt"  

//启用开关  
#define LOG_ENABLE  

#include <fstream>    
#include <string>    
#include <ctime>    
#include "GlbTime.h"
//#include "GlbWString.h"
//#include "GlbPath.h"

#include <comutil.h>
#pragma comment(lib, "comsuppw.lib") 

using namespace std;  

class GLB_DLLCLASS_EXPORT CGlbGlobeLog
{
public:
	CGlbGlobeLog(void);
	~CGlbGlobeLog(void);

public:    
	static void GetLogFilePath(CHAR* szPath)  
	{  
#if 1
		HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
		GetModuleFileNameA( hmd, szPath, MAX_PATH ) ;  
		ZeroMemory(strrchr(szPath,'\\'), strlen(strrchr(szPath,'\\' ) )*sizeof(CHAR)) ;  
		strcat(szPath,"\\");  
		strcat(szPath,LOG_FILE_NAME);  	
#else
		HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");	
		CGlbWString mdir = CGlbPath::GetModuleDir(hmd);
		CGlbString _dir = mdir.ToString() + "\\" + LOG_FILE_NAME;
		strcpy(szPath,_dir.c_str());
		hmd = NULL;
		//szPath = _dir.c_str();
#endif
	}  
	//输出一个内容，可以是字符串(ascii)、整数、浮点数、布尔、枚举  
	//格式为：[2011-11-11 11:11:11] aaaaaaa并换行  
	template <class T>  
	static void WriteLog(T x)  
	{  
		CHAR szPath[MAX_PATH] = {0};  
		GetLogFilePath(szPath);  

		ofstream fout(szPath,ios::app);  
		fout.seekp(ios::end);  
		fout << GetSystemTime() << x <<endl;  
		fout.close();  
	}  

	//输出2个内容，以等号连接。一般用于前面是一个变量的描述字符串，后面接这个变量的值  
	template<class T1,class T2>   
	static void WriteLog2(T1 x1,T2 x2)  
	{  
		CHAR szPath[MAX_PATH] = {0};  
		GetLogFilePath(szPath);  
		ofstream fout(szPath,ios::app);  
		fout.seekp(ios::end);  
		fout << GetSystemTime() << x1 <<" = "<<x2<<endl;  
		fout.close();  
	}  

	//输出一行当前函数开始的标志,宏传入__FUNCTION__  
	template <class T>  
	static void WriteFuncBegin(T x)  
	{  
		CHAR szPath[MAX_PATH] = {0};  
		GetLogFilePath(szPath);  
		ofstream fout(szPath,ios::app);  
		fout.seekp(ios::end);  
		fout << GetSystemTime() << "    --------------------"<<x<<"  Begin--------------------" <<endl;  
		fout.close();  
	}  

	//输出一行当前函数结束的标志，宏传入__FUNCTION__  
	template <class T>  
	static void WriteFuncEnd(T x)  
	{  
		CHAR szPath[MAX_PATH] = {0};  
		GetLogFilePath(szPath);  
		ofstream fout(szPath,ios::app);  
		fout.seekp(ios::end);  
		fout << GetSystemTime() << "--------------------"<<x<<"  End  --------------------" <<endl;  
		fout.close();  
	}  
	
	// * @param format  格式字符串
	static bool OutputLogString(WCHAR* format,...)
	{
		std::string _strTime = GetSystemTime();
		//采用ATL封装_bstr_t的过渡 实现char* -> wchar*
		_bstr_t t = _strTime.c_str();
		wchar_t* pwchar = (wchar_t*)t;
		std::wstring _wstrTime = pwchar;

		glbInt32 nlen = GLBSTRBUFFERSIZE*sizeof(WCHAR);
		WCHAR* info = (WCHAR*)malloc(nlen);
		if (NULL == info)
		{
			return false;
		}
		va_list args;
		va_start(args, format);
		glbInt32 nretval = vswprintf(info, nlen, format, args);
		va_end(args);
		//如果构造字符串，内存溢出，返回负值
		if (nretval >= 0)
		{
			_wstrTime += info;
		}
		free(info);

		OutputDebugString(_wstrTime.c_str());

		return true;
	}

	//// * @param format  格式字符串
	//static bool OutputLogString(glbWChar* format,...)
	//{
	//	CGlbString _strTime = GetSystemTime();
	//	CGlbWString wstr = _strTime.ToWString();	

	//	glbInt32 nlen = GLBSTRBUFFERSIZE*sizeof(glbWChar);
	//	glbWChar* info = (glbWChar*)malloc(nlen);
	//	if (NULL == info)
	//	{
	//		return false;
	//	}
	//	va_list args;
	//	va_start(args, format);
	//	glbInt32 nretval = vswprintf(info, nlen, format, args);
	//	va_end(args);
	//	//如果构造字符串，内存溢出，返回负值
	//	if (nretval >= 0)
	//	{
	//		wstr += info;
	//	}
	//	free(info);

	//	OutputDebugString(wstr.c_str());

	//	return true;
	//}

private:  
	//获取本地时间，格式如"[2011-11-11 11:11:11] ";   
	static string GetSystemTime()    
	{    
		time_t tNowTime;    
		time(&tNowTime);    
		tm* tLocalTime = localtime(&tNowTime);    
		char szTime[30] = {'\0'};    
		strftime(szTime, 30, "[%Y-%m-%d %H:%M:%S] ", tLocalTime);    
		string strTime = szTime;    
		return strTime;    
	}    
};

