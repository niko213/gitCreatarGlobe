/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbBase64.h
  * @brief   Base64编解码类头文件
  *
  * 这个档案定义CGlbBase64这个类
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 9:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbCommTypes.h"

/**
  * @brief CGlbBase64编码类
  *
  * 该类把二进制数据编码成字符串，及把字符串解码成二进制数据。
  * 方便网络传输
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 9:40
  */
class GLBCOMM_API CGlbBase64
{
public:
/**
  * @brief Base64编码
  *
  * 静态函数，把二进制数据编码成字符串
  *
  * @param indata 需要编码的二进制数据
  * @param inlen  需要编码的数据字节长度
  * @param oudata 输出参数，编码的字符串
  * @param outlen 输出参数，编码后的字节长度
  * @return true表示成功，false表示失败
  */
	static glbBool Encode (const glbByte* indata, glbInt32 inlen, glbChar** oudata, glbInt32& outlen);
/**
  * @brief Base64解码
  *
  * 静态函数，把字符串解码成二进制数据
  *
  * @param indata 需要解码的字符串
  * @param inlen  需要解码的字节长度
  * @param oudata 输出参数，解码的二进制数据
  * @param outlen 输出参数，解码后的字节长度
  * @return true表示成功，false表示失败
  */
	static glbBool Decode (const glbChar* indata, glbInt32 inlen, glbByte** oudata, glbInt32& outlen);
};

