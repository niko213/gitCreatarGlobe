// stdafx.cpp : source file that includes just the standard includes
// GlbGlobeObject.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

osg::Vec4 GetColor( glbInt32 color )
{
	glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
	glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
	glbInt32 blue = LOBYTE(color);//GetRValue(color);
	glbInt32 alpha = LOBYTE((color)>>24);//GetAValue(color);

	return osg::Vec4(red / 255.0,green / 255.0,blue / 255.0,alpha / 255.0);
}

osg::ref_ptr<osg::EllipsoidModel> g_ellipsoidModel = new osg::EllipsoidModel(osg::WGS_84_RADIUS_EQUATOR,osg::WGS_84_RADIUS_POLAR);

//CGlbObjectRegisterCreator *g_creator = new CGlbObjectRegisterCreator();
CGlbObjectRegisterCreator g_creator;// = new CGlbObjectRegisterCreator();//modified by longtingyou

//代码转换:从一种编码转为另一种编码  
int code_convert_obj(char* from_charset, char* to_charset, char* inbuf,	int inlen, char* outbuf, int outlen)
{
	iconv_t cd;
	char** pin = &inbuf;  
	char** pout = &outbuf;
	cd = iconv_open(to_charset,from_charset);  

	if(cd == 0)
		return -1;

	memset(outbuf,0,outlen); 
	size_t _inlen = inlen;
	size_t _outlen = outlen;

	//if(iconv(cd,(const char**)pin,(unsigned int /*size_t*/ *)&inlen,pout,(unsigned int /*size_t*/ *)&outlen)
	//	== -1)
	if(iconv(cd,(const char**)pin,&_inlen,pout,&_outlen) == -1)
		return -1;  
	iconv_close(cd);
	return 0;  
}

//UNICODE码转为GB2312码  
//成功则返回一个动态分配的char*变量，需要在使用完毕后手动free，失败返回NULL
char* u2g_obj(char *inbuf)  
{
	int nOutLen = 2 * strlen(inbuf) - 1;
	if(nOutLen<=0)return NULL;
	char* szOut = (char*)malloc(nOutLen);
	if (-1 == code_convert_obj("utf-8","gb2312",inbuf,strlen(inbuf),szOut,nOutLen))
	{
		free(szOut);
		szOut = NULL;
	}
	return szOut;
}  

//获取系统剩余内存
#include <Psapi.h>
glbInt64 GetUnusedMemory()
{
	glbInt64 mem = 0;
#ifdef _WIN64
	MEMORYSTATUS memstatus;
	memset(&memstatus,0,sizeof(MEMORYSTATUS));
	memstatus.dwLength =sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memstatus);
	mem=memstatus.dwAvailPhys ;	
#else
	PROCESS_MEMORY_COUNTERS pmc;  
    if(GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
	{
		mem = pmc.WorkingSetSize;//进程已使用物理内存
		mem +=pmc.PagefileUsage; //加上进程使用的虚拟内存
		glbInt64 totalCanUse = 2*1024;
		totalCanUse = totalCanUse*1024*1024;
		mem = totalCanUse - mem;	  //进程可用剩余内存	
	}
#endif
	return mem;
}

//获取本地时间，格式如"[2011-11-11 11:11:11] ";   
std::string GetSystemTime()    
{    
	time_t tNowTime;    
	time(&tNowTime);    
	tm* tLocalTime = localtime(&tNowTime);    
	char szTime[30] = {'\0'};    
	strftime(szTime, 30, "[%Y-%m-%d %H:%M:%S] ", tLocalTime);    
	std::string strTime = szTime;    
	return strTime;    
}    