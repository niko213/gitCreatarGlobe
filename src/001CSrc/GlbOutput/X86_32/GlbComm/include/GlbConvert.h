/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbConvert.h
  * @brief   字符串与其它数值类型转换定义文件
  *
  * 这个档案定义CGlbConvert这个class,为方便使用，全部是静态函数
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 9:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include "GlbString.h"
#include "GlbWString.h"
/**
  * @brief CGlbConvert类型转换类
  *
  * 其成员函数全部是静态函数，方便调用
  * 实现了字符串转int16、int32、int64、float、double
  *及int16、int32、int64、float、double转字符串
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-6 16:40
  */
class GLBCOMM_API CGlbConvert
{
public:
/**
  * @brief 窄字符串转int16
  *
  * 静态函数，如果不是数字或者超过16位整数范围，报错
  *
  * @param src 要转换的字符串
  * @param num 转换后的int16数字
  * @return 成功返回true，失败返回false
  */
	static bool ToInt16 (const glbChar* src, glbInt16& num);
/**
  * @brief 窄字符串转int32
  *
  * 静态函数，如果不是数字，报错
  *
  * @param src 要转换的字符串
  * @param num 转换后的int32数字
  * @return 成功返回true，失败返回false
  */
	static bool ToInt32 (const glbChar* src, glbInt32& num);
/**
  * @brief 窄字符串转int64
  *
  * 静态函数，如果不是数字，报错
  *
  * @param src 要转换的字符串
  * @param num 转换后的int64数字
  * @return 成功返回true，失败返回false
  */
	static bool ToInt64 (const glbChar* src, glbInt64& num);
/**
  * @brief 窄字符串转float
  *
  * 静态函数，如果不是数字或者超过float范围，报错
  *
  * @param src 要转换的字符串
  * @param num 转换后的float数字
  * @return 成功返回true，失败返回false
  */
	static bool ToFloat (const glbChar* src, glbFloat& num);
/**
  * @brief 窄字符串转double
  *
  * 静态函数，如果不是数字，报错
  *
  * @param src 要转换的字符串
  * @param num 转换后的double数字
  * @return 成功返回true，失败返回false
  */
	static bool ToDouble (const glbChar* src, glbDouble& num);
/**
  * @brief 宽字符串转int16
  *
  * 静态函数，如果不是数字或者超过16位整数范围，报错
  *
  * @param src 要转换的字符串
  * @param num 转换后的int16数字
  * @return 成功返回true，失败返回false
  */
	static bool ToInt16 (const glbWChar* src, glbInt16& num);
/**
  * @brief 宽字符串转int32
  *
  * 静态函数，如果不是数字，报错
  *
  * @param src 要转换的字符串
  * @param num 转换后的int32数字
  * @return 成功返回true，失败返回false
  */
	static bool ToInt32 (const glbWChar* src, glbInt32& num);
/**
  * @brief 宽字符串转int64
  *
  * 静态函数，如果不是数字，报错
  *
  * @param src 要转换的字符串
  * @param num 转换后的int64数字
  * @return 成功返回true，失败返回false
  */
	static bool ToInt64 (const glbWChar* src, glbInt64& num);
/**
  * @brief 宽字符串转Float
  *
  * 静态函数，如果不是数字或者超过Float范围，报错
  *
  * @param src 要转换的字符串
  * @param num 转换后的Float数字
  * @return 成功返回true，失败返回false
  */
	static bool ToFloat (const glbWChar* src, glbFloat& num);
/**
  * @brief 宽字符串转Double
  *
  * 静态函数，如果不是数字或者操作Double浮点数，报错
  *
  * @param src 要转换的字符串
  * @param num 转换后的Double数字
  * @return 成功返回true，失败返回false
  */
	static bool ToDouble (const glbWChar* src, glbDouble& num);

/**
  * @brief int16转窄字符串
  *
  * 静态函数，把int16转成窄字符串输出
  *
  * @param num 需要转换数值
  * @return 成功返回字符串，失败返回NULL
  */
	//static glbChar* Int16ToStr (glbInt16 num);
	static CGlbString Int16ToStr (glbInt16 num);
/**
  * @brief int32转窄字符串
  *
  * 静态函数，把int32转成窄字符串输出
  *
  * @param num 需要转换数值
  * @return 成功返回字符串，失败返回NULL
  */
	//static glbChar* Int32ToStr (glbInt32 num);
	static CGlbString Int32ToStr (glbInt32 num);
/**
  * @brief int64转窄字符串
  *
  * 静态函数，把int64转成窄字符串输出
  *
  * @param num 需要转换数值
  * @return 成功返回字符串，失败返回NULL
  */
	//static glbChar* Int64ToStr (glbInt64 num);
	static CGlbString Int64ToStr (glbInt64 num);
/**
  * @brief float转窄字符串
  *
  * 静态函数，把float转成窄字符串输出
  *
  * @param num 需要转换数值
  * @return 成功返回字符串，失败返回NULL
  */
	//static glbChar* FloatToStr (glbFloat num);
	static CGlbString FloatToStr (glbFloat num);
/**
  * @brief double转窄字符串
  *
  * 静态函数，把double转成窄字符串输出
  *
  * @param num 需要转换数值
  * @return 成功返回字符串，失败返回NULL
  */
	//static glbChar* DoubleToStr (glbDouble num);
	static CGlbString DoubleToStr (glbDouble num);
/**
  * @brief int16转宽字符串
  *
  * 静态函数，把int16转成宽字符串输出
  *
  * @param num 需要转换数值
  * @return 成功返回字符串，失败返回NULL
  */
	//static glbWChar* Int16ToWStr (glbInt16 num);
	static CGlbWString Int16ToWStr (glbInt16 num);
/**
  * @brief int32转宽字符串
  *
  * 静态函数，把int32转成宽字符串输出
  *
  * @param num 需要转换数值
  * @return 成功返回字符串，失败返回NULL
  */
	//static glbWChar* Int32ToWStr (glbInt32 num);
	static CGlbWString Int32ToWStr (glbInt32 num);
/**
  * @brief int64转宽字符串
  *
  * 静态函数，把int64转成宽字符串输出
  *
  * @param num 需要转换数值
  * @return 成功返回字符串，失败返回NULL
  */
	//static glbWChar* Int64ToWStr (glbInt64 num);
	static CGlbWString Int64ToWStr (glbInt64 num);
/**
  * @brief float转宽字符串
  *
  * 静态函数，把float转成宽字符串输出
  *
  * @param num 需要转换数值
  * @return 成功返回字符串，失败返回NULL
  */
	//static glbWChar* FloatToWStr (glbFloat num);
	static CGlbWString FloatToWStr (glbFloat num);
/**
  * @brief double转宽字符串
  *
  * 静态函数，把double转成宽字符串输出
  *
  * @param num 需要转换数值
  * @return 成功返回字符串，失败返回NULL
  */
	//static glbWChar* DoubleToWStr (glbDouble num);
	static CGlbWString DoubleToWStr (glbDouble num);
};

