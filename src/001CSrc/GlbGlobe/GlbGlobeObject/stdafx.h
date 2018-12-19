// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

//使用std::string::npos做为find查询结果是否存在判断标准时，如果不加下面的这几句，编译器提示以下错误: 2018.9.15
//error LNK2001: unresolved external symbol "public: static unsigned int const std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::npos"
#include <xstring>
template std::string::size_type std::string::npos;
template std::wstring::size_type std::wstring::npos;
#if _MSC_VER >= 1600 //vs10.0
const std::basic_string<char>::size_type std::basic_string<char>::npos = (std::basic_string<char>::size_type) -1;
const std::basic_string<wchar_t>::size_type std::basic_string<wchar_t>::npos = (std::basic_string<wchar_t>::size_type) -1;
#endif

#include "osg/Vec4"
#include "GlbCommTypes.h"
#define GetAValue(rgba)      (LOBYTE((rgba)>>24))
extern osg::Vec4 GetColor(glbInt32 color);

#define GetRValueInRGBA(rgba)      (LOBYTE(rgba))
#define GetGValueInRGBA(rgba)      (LOBYTE(((WORD)(rgba)) >> 8))
#define GetBValueInRGBA(rgba)      (LOBYTE((rgba)>>16))
#define GetRValueInBGRA(bgra)      (LOBYTE((bgra)>>16))
#define GetGValueInBGRA(bgra)      (LOBYTE(((bgra & 0xFFFF)) >> 8))//(LOBYTE(((WORD)(bgra)) >> 8))
#define GetBValueInBGRA(bgra)      (LOBYTE(bgra))

#include "osg/CoordinateSystemNode"
extern osg::ref_ptr<osg::EllipsoidModel> g_ellipsoidModel;

// TODO: reference additional headers your program requires here

#include "GlbObjectRegisterCreator.h"
#include "GlbGlobeREObject.h"
//extern CGlbObjectRegisterCreator* g_creator;
extern CGlbObjectRegisterCreator g_creator;//modified by longtingyou
//代码转换:从一种编码转为另一种编码  
int code_convert_obj(char* from_charset, char* to_charset, char* inbuf, int inlen, char* outbuf, int outlen);
//UNICODE码转为GB2312码  
//成功则返回一个动态分配的char*变量，需要在使用完毕后手动free，失败返回NULL
char* u2g_obj(char *inbuf);
//日志
//#include "GlbLog.h"

//我的日志
#include "GlbGlobeLog.h"
#ifdef LOG_ENABLE  
//用下面这些宏来使用本文件  
#define LOG(x)          CGlbGlobeLog::WriteLog(x);          //括号内可以是字符串(ascii)、整数、浮点数、bool等  
#define LOG2(x1,x2)     CGlbGlobeLog::WriteLog2(x1,x2);  
#define LOG_FUNC        LOG(__FUNCTION__)               //输出当前所在函数名  
#define LOG_LINE        LOG(__LINE__)                       //输出当前行号  
#define LOG_FUNC_BEGIN  CGlbGlobeLog::WriteFuncBegin(__FUNCTION__);     //形式如：[时间]"------------FuncName  Begin------------"  
#define LOG_FUNC_END    CGlbGlobeLog::WriteFuncEnd(__FUNCTION__);      //形式如：[时间]"------------FuncName  End------------" 
#else  
#define LOG(x)                
#define LOG2(x1,x2)       
#define LOG_FUNC          
#define LOG_LINE          
#define LOG_FUNC_BEGIN    
#define LOG_FUNC_END      
#endif  

std::string GetSystemTime();    

//获取剩余内存
extern glbInt64 GetUnusedMemory();
/*
	下面这个宏，是为了使用ffmpeg而定义!!!!!.
*/
#define __STDC_CONSTANT_MACROS
