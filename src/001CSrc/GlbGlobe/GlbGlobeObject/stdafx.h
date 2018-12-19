// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

//ʹ��std::string::npos��Ϊfind��ѯ����Ƿ�����жϱ�׼ʱ���������������⼸�䣬��������ʾ���´���: 2018.9.15
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
//����ת��:��һ�ֱ���תΪ��һ�ֱ���  
int code_convert_obj(char* from_charset, char* to_charset, char* inbuf, int inlen, char* outbuf, int outlen);
//UNICODE��תΪGB2312��  
//�ɹ��򷵻�һ����̬�����char*��������Ҫ��ʹ����Ϻ��ֶ�free��ʧ�ܷ���NULL
char* u2g_obj(char *inbuf);
//��־
//#include "GlbLog.h"

//�ҵ���־
#include "GlbGlobeLog.h"
#ifdef LOG_ENABLE  
//��������Щ����ʹ�ñ��ļ�  
#define LOG(x)          CGlbGlobeLog::WriteLog(x);          //�����ڿ������ַ���(ascii)����������������bool��  
#define LOG2(x1,x2)     CGlbGlobeLog::WriteLog2(x1,x2);  
#define LOG_FUNC        LOG(__FUNCTION__)               //�����ǰ���ں�����  
#define LOG_LINE        LOG(__LINE__)                       //�����ǰ�к�  
#define LOG_FUNC_BEGIN  CGlbGlobeLog::WriteFuncBegin(__FUNCTION__);     //��ʽ�磺[ʱ��]"------------FuncName  Begin------------"  
#define LOG_FUNC_END    CGlbGlobeLog::WriteFuncEnd(__FUNCTION__);      //��ʽ�磺[ʱ��]"------------FuncName  End------------" 
#else  
#define LOG(x)                
#define LOG2(x1,x2)       
#define LOG_FUNC          
#define LOG_LINE          
#define LOG_FUNC_BEGIN    
#define LOG_FUNC_END      
#endif  

std::string GetSystemTime();    

//��ȡʣ���ڴ�
extern glbInt64 GetUnusedMemory();
/*
	��������꣬��Ϊ��ʹ��ffmpeg������!!!!!.
*/
#define __STDC_CONSTANT_MACROS
