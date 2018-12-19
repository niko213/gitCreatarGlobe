/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbCompress.h
  * @brief   压缩解压头文件
  *
  * 这个档案定义CGlbCompress这个类
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-12 16:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbCommTypes.h"

typedef struct _GlbBmpDataBuffer
{
	int      width;
	int      height;
	glbByte* data;
} GlbBmpDataBuffer;
/**
  * @brief CGlbCompress压缩解压类
  *
  * 根据压缩类型，选择对应的压缩解压算法进行压缩解压
  * 函数全部静态实现
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 14:40
  */
class GLBCOMM_API CGlbCompress
{
public:
/**
  * @brief 压缩数据
  *
  * 静态函数，根据压缩类型，压缩数据
  *
  * @param type   压缩类型
  * @param indata 需要压缩的二进制数据
  * @param inlen  需要压缩的数据字节长度
  * @param oudata 输出参数，压缩后的二进制数据
  * @param outlen 输出参数，压缩后的字节长度
  * @return true表示成功，false表示失败
  */
	static glbBool Compress(GlbCompressTypeEnum type, 
		                    glbByte*            indata, 
							glbUInt32           inlen, 
							glbByte**           oudata, 
							glbUInt32&          outlen);
/**
  * @brief 解压数据
  *
  * 静态函数，根据压缩类型，解压数据
  *
  * @param type   压缩类型,当前支持zlib压缩
  * @param indata 需要解压的二进制数据
  * @param inlen  需要解压的数据字节长度
  * @param oudata 输出参数，解压后的二进制数据
  * @param outlen 输出参数，解压后的字节长度
  * @return true表示成功，false表示失败
  */
	static glbBool UnCompress(GlbCompressTypeEnum type, 
		                      glbByte* indata, 
							  glbUInt32 inlen, 
							  glbByte** oudata, 
							  glbUInt32& outlen);
private:
/**
  * @brief Zlib压缩数据
  *
  * 静态函数，利用Zlib压缩数据
  * 注意：由于解压时，需要提前对解压后数据申请内存，所以压缩后的数据
  * 前4个字节保存原始数据长度inlen
  *
  * @param indata 需要压缩的二进制数据
  * @param inlen  需要压缩的数据字节长度
  * @param oudata 输出参数，压缩后的二进制数据，前4个字节保存inlen
  * @param outlen 输出参数，压缩后的字节长度+4
  * @return true表示成功，false表示失败
  */
	static glbBool Zlib_Compress(glbByte* indata, glbUInt32 inlen, glbByte** oudata, glbUInt32& outlen);
/**
  * @brief 解压数据
  *
  * 静态函数，利用Zlib解压数据
  * 注意：解压的数据前4个字节保存的原始数据长度，也就是outlen，后面的才是数据
  *
  * @param indata 前4个字节是outlen，后面的需要解压的二进制数据
  * @param inlen  需要解压的数据字节长度+4
  * @param oudata 输出参数，解压后的二进制数据
  * @param outlen 输出参数，解压后的字节长度
  * @return true表示成功，false表示失败
  */
	static glbBool Zlib_UnCompress(glbByte* indata, glbUInt32 inlen, glbByte** oudata, glbUInt32& outlen);

	/**
  * @brief Jpeg压缩数据
  *
  * 静态函数，利用Jpeg9.lib压缩数据
  * 注意：由于压缩时，需要jpeg的相关信息，indata前 字节用作保存该信息
  * 
  *
  * @param indata 需要压缩的二进制数据
  * @param inlen  需要压缩的数据字节长度
  * @param oudata 输出参数，压缩后的二进制数据
  * @param outlen 输出参数，压缩后的字节长度
  * @return true表示成功，false表示失败
  */
	static glbBool Jpeg_Compress(glbByte* indata, glbUInt32 inlen, glbByte** oudata, glbUInt32& outlen);
/**
  * @brief 解压数据
  *
  * 静态函数，利用Jpeg解压数据
  * 注意：由于压缩时，需要jpeg的相关信息，indata前 字节用作保存该信息
  *
  * @param indata 需要解压的二进制数据
  * @param inlen  需要解压的数据字节长度
  * @param oudata 输出参数，解压后的二进制数据
  * @param outlen 输出参数，解压后的字节长度
  * @return true表示成功，false表示失败
  */
	static glbBool Jpeg_UnCompress(glbByte* indata, glbUInt32 inlen, glbByte** oudata, glbUInt32& outlen);
};



