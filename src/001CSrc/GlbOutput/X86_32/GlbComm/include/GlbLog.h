/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbLog.h
  * @brief   日志操作类头文件
  *
  * 这个档案定义CGlbLog这个接口类
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40
*********************************************************************/
#pragma once
#pragma warning(disable:4251)//不可取
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include "IGlbLogHandler.h"

#define GlbLogOutput g_glblog.Output   ///< 日志信息窄字符串输出宏定义
#define GlbLogWOutput g_glblog.WOutput ///< 日志信息宽字符串输出宏定义

/**
  * @brief CGlbLog日志类
  *
  * 该类把日志信息按照格式串输出到日志句柄中，
  * 另外维护日志句柄
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40 
  */
class GLBCOMM_API CGlbLog
{
public:
/**
  * @brief 构造函数
  *
  * 清除mpr_logHandlers数组
  *
  * @param logInfo 日志信息
  * @return 无
  */
	CGlbLog(void);
/**
  * @brief 析构函数
  *
  * 遍历mpr_logHandlers数组，如果有句柄存在，释放
  *
  * @return 无
  */
	virtual ~CGlbLog(void);

/**
  * @brief 窄字符日志信息输出函数
  *
  * 根据日志类型和输出格式，把日志信息输出
  *
  * @param logType 日志类型
  * @param format  格式字符串
  * @param ...     其他动态参数
  * @return true表示成功，false表示失败
  */
	glbBool Output(GlbLogTypeEnum logType, glbChar* format,...);
/**
  * @brief 宽字符日志信息输出函数
  *
  * 根据日志类型和输出格式，把日志信息输出
  *
  * @param logType 日志类型
  * @param format  格式字符串
  * @param ...     其他动态参数
  * @return true表示成功，false表示失败
  */
	glbBool WOutput(GlbLogTypeEnum logType, glbWChar* format,...);
/**
  * @brief 增加日志句柄
  *
  * 如果当前句柄存在，增加失败
  *
  * @param logHandler 日志句柄
  * @return true表示成功，false表示失败
  */
	glbBool AddLogHandler(IGlbLogHandler* logHandler);
/**
  * @brief 删除日志句柄
  *
  * 如果当前句柄不存在，删除失败
  *
  * @param logHandler 日志句柄
  * @return true表示成功，false表示失败
  */
	glbBool DeleteLogHandler(IGlbLogHandler* logHandler);

private:
	std::vector<IGlbLogHandler*> mpr_logHandlers;///< 日志句柄数组
};

extern GLBCOMM_API CGlbLog g_glblog;///< 全局变量，日志操作类
