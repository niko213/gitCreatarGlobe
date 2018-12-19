/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbTime.h
  * @brief   时间操作头文件
  *
  * 这个档案定义GlbGetNowTime、GlbTime2Str、GlbTime2WStr、GlbStr2Time、GlbStr2Time
  * 5个全局函数
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbCommTypes.h"

/**
  * @brief 获取当前系统时间
  *
  * 静态函数，获取当前系统时间，用64位整数表示
  *
  * @return 返回64位整数表示的时间
  */
extern GLBCOMM_API glbInt64 GlbGetNowTime();
/**
  * @brief 转换时间成窄字符串
  *
  * 静态函数，按照格式串转换时间成窄字符串
  *
  * @param time   要转换的时间
  * @param format 格式字符串 %d存在的个数应该大于0小于等于6
  * @return 成功返回字符串，失败返回NULL
  */
extern GLBCOMM_API glbChar* GlbTime2Str(glbInt64 time, const glbChar* format);
/**
  * @brief 转换时间成宽字符串
  *
  * 静态函数，按照格式串转换时间成宽字符串
  *
  * @param time   要转换的时间
  * @param format 格式字符串 %d存在的个数应该大于0小于等于6
  * @return 成功返回字符串，失败返回NULL
  */
extern GLBCOMM_API glbWChar* GlbTime2WStr(glbInt64 time, const glbWChar* format);
/**
  * @brief 把窄字符串转换成时间
  *
  * 静态函数，按照格式串把时间窄字符串转换成时间
  *
  * @param timestr   要转换的时间字符串
  * @param format    格式字符串 %d存在的个数应该大于0小于等于6
  * @param time      输出参数，转换后的时间
  * @return 成功返回true，失败返回false
  */
extern GLBCOMM_API glbBool GlbStr2Time(const glbChar* timestr, const glbChar* format, glbInt64* time);
/**
  * @brief 把宽字符串转换成时间
  *
  * 静态函数，按照格式串把时间宽字符串转换成时间
  *
  * @param timestr   要转换的时间字符串
  * @param format    格式字符串 %d存在的个数应该大于0小于等于6
  * @param time      输出参数，转换后的时间
  * @return 成功返回true，失败返回false
  */
extern GLBCOMM_API glbBool GlbWStr2Time(const glbWChar* timestr, const glbWChar* format, glbInt64* time);

