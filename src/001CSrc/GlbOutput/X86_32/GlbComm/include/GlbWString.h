/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbWString.h
  * @brief   字符串操作类头文件
  *
  * 这个档案定义CGlbWString这个class,
  * 注意：wstring的find_last_not_of、find_last_of无法使用，
  * 继承后find_last_not_of、find_last_of也不能使用
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 10:40
*********************************************************************/
#pragma once
#pragma warning(disable:4251)//不可取
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include <string>
using namespace std;

class CGlbString;///< 窄字符串定义，方便引用
/**
  * @brief CGlbWString宽字符串类
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 10:40
  */
//#if _MSC_VER >= 1600
//const std::wstring::size_type std::wstring::npos = (std::wstring::size_type) -1;
//#endif
class GLBCOMM_API CGlbWString :public std::wstring
{
public:
/**
  * @brief CGlbWString构造函数
  *
  * 没有任何参数及传回值
  */
	CGlbWString(void);
/**
  * @brief CGlbWString拷贝构造函数
  *
  * copy src字符串到该字符串
  *
  * @param src 用来拷贝的源字符串
  * @return 无
  */
	CGlbWString(const CGlbWString& src);
/**
  * @brief CGlbWString构造函数
  *
  * copy src字符串到该字符串
  *
  * @param src 用来拷贝的源字符串
  * @return 无
  */
	CGlbWString(const wstring& src);
/**
  * @brief CGlbWString构造函数
  *
  * copy psz字符串到该字符串,如果为NULL，mpr_str为空字符串
  *
  * @param psz 用来拷贝的源字符串
  * @return 无
  */
	CGlbWString(const glbWChar* psz);

	CGlbWString(const glbWChar* psz, glbInt32 count);
/**
  * @brief 转换为窄字符串
  * @brief 窄字符编码格式为操作系统当前默认代码页
  *
  * @return 转换后的窄字符串
  */
	CGlbString ToString();
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
  * @brief 转换为UTF8窄字符串
  */
	CGlbString ToUTF8String();
/**
  * @brief 分割
*/
	vector<CGlbWString> Split(glbWChar split);
/**
  * @brief 按照特定格式构造字符串
  *  
  * 如果失败，返回一个空字符串
  *
  * @param format 格式化字符串
  * @param ...    其他动态参数
  * @return 字符串类
  */
	static CGlbWString FormatString(glbWChar* format, ...);
};

