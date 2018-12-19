/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    IGlbLogHandler.h
  * @brief   日志句柄类头文件
  *
  * 这个档案定义IGlbLogHandler这个接口类,
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"

/**
  * @brief CGlbLog日志句柄类
  *
  * 是一个接口，使用者实现该接口，把日志信息输出
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40
  */
class GLBCOMM_API IGlbLogHandler
{
public:
/**
  * @brief 日志信息输出
  *
  * 把日志信息输出终端
  *
  * @param logInfo 日志信息
  * @return 无
  */
	virtual void Output(const wchar_t* logInfo)=0;
};

