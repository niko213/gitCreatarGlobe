/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbFontPath.h
  * @brief   字体路径对应头文件
  *
  * 这个档案定义CGlbFontPath这个类
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-14 9:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include "GlbWString.h"

/**
  * @brief CGlbFontPath字体路径类
  *
  * 获取系统字体名及其对应的路径名
  * 单一实例
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-14 9:40
  */
class GLBCOMM_API CGlbFontPath
{
private:
/**
  * @brief 构造函数
  *
  * 私有，不允许构造实例
  * 设置系统字体文件目录mpr_fontdir，
  * 设置字体名和字体文件名的键值对mpr_mapfontPaths
  *
  * @return 无
  */
	CGlbFontPath(void);
/**
  * @brief 析构函数
  *
  * @return 无
  */
	~CGlbFontPath(void){}
/**
  * @brief 根据文件全路径获取字体名
  *
  * 获取字体名
  *
  * @param strfilepath 文件全路径
  * @return 字体名，出错，返回空字符串
  */
	CGlbWString GetFontNameFromFile(CGlbWString strfilepath);
	static CGlbFontPath mpr_instance;///<字体路径唯一静态实例，声明
public:
/**
  * @brief 获取一个实例
  *
  * 静态函数 获取一个实例
  *
  * @return 一个字体路径实例
  */
	static CGlbFontPath* GetInstance();
/**
  * @brief 根据字体名获取文件全路径
  *
  * 获取文件全路径
  *
  * @param fontName 字体名
  * @return 字体名，出错，返回空字符串
  */
	//const glbWChar* GetFontFileFullName(const glbWChar* fontName);
	CGlbWString GetFontFileFullName(const glbWChar* fontName);
	/**
	  * @brief 获取字体数目
	  *	  
	  * @return 字体数目
	*/
	glbInt32 GetCount();
	/**
	  * @brief 获取字体名称
	  *	  
	  * @param idx 索引
	  * @return 字体名称，如果不存在返回空字符串
	*/
	CGlbWString GetFontName(glbInt32 idx);

private:
	std::map<CGlbWString /*fontname*/, CGlbWString /*fontFileFullName*/> mpr_mapfontPaths;///<字体名与文件全路径键值对
	CGlbWString mpr_fontdir;///<系统字体目录
};

