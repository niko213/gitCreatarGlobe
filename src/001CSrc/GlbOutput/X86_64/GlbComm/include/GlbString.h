/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbString.h
  * @brief   字符串操作类头文件
  *
  * 这个档案定义CGlbString这个class,
  * GlbChar2WChar、GlbWChar2Char、GlbChar2UTF8、WChar2UTF8四个全局函数
  *
  * 注意：string的find_last_not_of、find_last_of无法使用，
  * 继承后find_last_not_of、find_last_of也不能使用
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-6 16:40
*********************************************************************/
#pragma once
#pragma warning(disable:4251)//不可取
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include <string>
using namespace std;
#ifdef  __cplusplus
extern "C" {
#endif
/**
  * @brief 窄字符串转成宽字符串
  * @brief 释放返回的字符串，用free
  *
  * @param srcstr    用来转换的窄源字符串
  * @param codepage  srcstr的编码页，缺省为操作系统当前编码页
  * @return          传回转换后的宽字符串
  */
extern GLBCOMM_API glbWChar* GlbChar2WChar(const glbChar* srcstr,const glbUInt32 codepage = CP_ACP);
/**
  * @brief 宽字符串转成窄字符串(windows 宽字符是UTF_16)
  * @brief 释放返回的字符串，用free
  *
  * @param   srcwstr 用来转换的宽源字符串
  * @param   codepage 返回值的编码页
  * @return  传回转换后的窄字符串
  */
extern GLBCOMM_API glbChar* GlbWChar2Char(const glbWChar* srcwstr,const glbUInt32 codepage = CP_ACP);
/**
  * @brief 宽字符串转成窄字符串{UTF8编码}(windows 宽字符是UTF_16)
  * @brief 释放返回的字符串，用free
  *
  * @param   srcwstr 用来转换的宽源字符串
  * @return  传回转换后的UTF8编码窄字符串
  */
extern GLBCOMM_API glbChar* GlbWChar2UTF8(const glbWChar* srcwstr);
/**
 * @brief 系统缺省编码窄字符串转换为UTF8编码窄字符串
 * @brief 释放返回的字符串，用free
  *
  * @param   srcstr 用来转换的系统缺省编码窄字符串
  * @return  传回转换后的UTF8编码窄字符串
 */
extern GLBCOMM_API glbChar* GlbChar2UTF8(const glbChar* srcstr);

extern GLBCOMM_API glbWChar* UTF82WChar(const glbChar* utf8);

#ifdef  __cplusplus
}
#endif
class CGlbWString; ///< 宽字符串定义，方便引用

/**
  * @brief CGlbString窄字符串类
  *					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-6 16:40
  */
class GLBCOMM_API CGlbString : public std::string
{
public:
	CGlbString(void);
	CGlbString(const CGlbString& src);
	CGlbString(const std::string& src);
	CGlbString(const glbChar* psz);
	CGlbString(const glbChar* psz, glbInt32 count);
/**
  * @brief 转换为宽字符串
  * @brief CGlbString 的字符编码如果与操作系统当前代码页不符，转换后是乱码. 
  *
  * @return 转换后的宽字符串
  */
	CGlbWString ToWString();
/**
  * @brief 把该字符串的字母全部转为大写
  *
  * 遍历整个字符串，如果是字母，全部转为大写，方便字符串的比较
  *
  * @return 成功返回true，失败返回false
  */
	glbBool ToUpper();
/**
  * @brief 把该字符串的字母全部转为小写
  *
  * 遍历整个字符串，如果是字母，全部转为小写，方便字符串的比较
  *
  * @return 成功返回true，失败返回false
  */
	glbBool ToLower();
/**
  * @brief 清除该字符串前面的空格
  *
  * 从头遍历整个字符串，如果是空格，清除
  *
  * @return 成功返回true，失败返回false
  */
	glbBool TrimLeft();
/**
  * @brief 清除该字符串后面的空格
  *
  * 从头遍历整个字符串，如果是空格，记录当前位置，如果后面都是空格，把当前位置字符改为结束符
  *
  * @return 成功返回true，失败返回false
  */
	glbBool TrimRight();
/**
  * @brief 清除该字符串前后两端的空格
  *
  * 调用TrimLeft和TrimRight进行处理
  *
  * @return 成功返回true，失败返回false
  */
	glbBool Trim();
/**
  * @brief 替换
  *
  * @param rpstr   需要替换的字符串
  * @param newstr  用来替换的字符串
  *
  **/
	void ReplaceAll(const CGlbString& rpstr,const CGlbString& newstr);

/**
  * @brief 按照特定格式构造字符串
  *
  * 静态函数。调用系统函数vsnprintf构造字符串，字符串最大长度GLBSTRBUFFERSIZE
  * 如果失败，返回一个空字符串
  *
  * @param format 格式化字符串
  * @param ...    其他动态参数
  * @return 字符串类
  */
	static CGlbString FormatString(glbChar* format, ...);
};

