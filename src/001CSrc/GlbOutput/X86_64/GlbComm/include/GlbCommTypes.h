/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbCommType.h
  * @brief   类型定义
  *
  * 这个档案定义GlbAltitudeModeEnum、GlbTextAlignmentEnum、GlbTextAxisAlignmentEnum
  * GlbFontSizeModeEnum、GlbLogTypeEnum、GlbCompressTypeEnum枚举类型，
  * glbVector、glbBlob、glbArray结构体类型，及基础数据类型重新定义。
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 14:40
*********************************************************************/
#pragma once
/**
  * @brief GlbLogTypeEnum 日志类型枚举
  *
  * 定义2种日志类型，一种普通错误信息，一种是错误信息				 
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40
  */
enum GlbLogTypeEnum
{
	GLB_LOGTYPE_INFO=0,///<日志普通信息
	GLB_LOGTYPE_ERR =1 ///<日志错误信息
};
/**
  * @brief GlbCompressTypeEnum压缩类型枚举
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-12 16:40
  */
enum GlbCompressTypeEnum
{
	GLB_COMPRESSTYPE_NO   = 0, ///<不压缩
	GLB_COMPRESSTYPE_LZW  = 1, ///<lzw压缩
	GLB_COMPRESSTYPE_ZLIB = 2, ///<zlib压缩
	GLB_COMPRESSTYPE_JPEG = 3 ///<jpeg压缩	
};
/**
  * @brief 32,64位编码注意事项
  *
  * 1.数据类型     LP32    LP64
  * char      8       8
  * short     16      16
  * int       32      32 
  * longlong  64      64
  * long      32      64
  * 指针      32       64
  * time_t    32       64
  * size_t    32       64
  * 2.取类型位数，使用sizeof，而不是直接写数字
  * 用sizeof(long)代替32
  * 3.%x（十六进制 int 格式）printf 修饰符在 64 位 Windows 操作系统中不按预期的那样工作；它只对传递给它的值的前 32 位进行操作。
  * Windows 32 位操作系统使用 %I32x 显示整数。
  * Windows 64 位操作系统使用 %I64x 显示整数。
  * %p（指针的十六进制格式）在 64 位 Windows 操作系统中按预期的那样工作。
  * 4.#ifdef _WIN64
  * 
  * 5. 系统API 32/64 位两个版本			 
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 14:40
  */
typedef __int8  glbInt8  ;///<8位有符号整数
typedef __int16 glbInt16 ;///<16位有符号整数
typedef __int32 glbInt32 ;///<32位有符号整数
typedef __int64 glbInt64 ;///<64位有符号整数
typedef bool    glbBool;  ///<布尔
typedef char    glbChar;  ///<字符
typedef wchar_t glbWChar; ///<宽字符
typedef float   glbFloat; ///<单精度浮点数
typedef double  glbDouble;///<双精度浮点数
typedef glbInt64  glbDate;  ///<时间？是不是用glbInt64
typedef unsigned __int8  glbUInt8; ///<8位无符号整数
typedef unsigned __int16 glbUInt16;///<16位无符号整数
typedef unsigned __int32 glbUInt32;///<32位无符号整数
typedef unsigned __int64 glbUInt64;///<64位无符号整数
typedef unsigned __int8  glbByte;  ///<字节

/**
  * @brief glbVector 向量结构体
  *			 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 14:40
  */
typedef struct _GLBVECTOR3D
{
	glbDouble x;///<x方向大小
	glbDouble y;///<y方向大小
	glbDouble z;///<z方向大小
}glbVector3d;

/**
  * @brief glbVector 向量结构体
  *			 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 14:40
  */
typedef struct  glbVector
{
	/** Data type of vector components.*/
	typedef double value_type;
public:
	/** Constructor that sets all components of the vector to zero */
	glbVector() { x=0.0f; y=0.0f; z=0.0f;}
	glbVector(value_type _x,value_type _y,value_type _z) { x=_x; y=_y; z=_z; }
	inline bool operator == (const glbVector& v) const { return x==v.x && y==v.y && z==v.z; }

	inline bool operator != (const glbVector& v) const { return x!=v.x || y!=v.y || z!=v.z; }
	/** Dot product. */
	inline value_type operator * (const glbVector& v) const
	{
		return x*v.x+y*v.y+z*v.z;
	}
	/** Cross product. */
	inline const glbVector operator ^ (const glbVector& v) const
	{
		return glbVector(y*v.z-z*v.y,
			z*v.x-x*v.z ,
			x*v.y-y*v.x);
	}
	/** Multiply by scalar. */	
	inline const glbVector operator *(value_type rhs)
	{
		return glbVector(x*rhs, y*rhs, z*rhs);
	}
	/** Unary multiply by scalar. */
	inline glbVector& operator *= (value_type rhs)
	{
		x*=rhs;
		y*=rhs;
		z*=rhs;
		return *this;
	}
	/** Binary vector add. */
	inline const glbVector operator + (const glbVector& v)
	{
		return glbVector(x+v.x, y+v.y, z+v.z);
	}
	 /** Unary vector add. Slightly more efficient because no temporary
          * intermediate object.
        */
    inline glbVector& operator += (const glbVector& v)
    {
        x += v.x;
		y += v.y;
        z += v.z;
        return *this;
    }
	/** Binary vector subtract. */
	inline const glbVector operator - (const glbVector& v)
	{
		return glbVector(x-v.x, y-v.y, z-v.z);
	}
	/** Unary vector subtract. */
	inline glbVector& operator -= (const glbVector& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	///** Length of the vector = sqrt( vec . vec ) */
	//inline value_type length() const
	//{
	//	return sqrt( x*x + y*y + z*z );
	//}
	/** Normalize the vector so that it has length unity.
        * Returns the previous length of the vector.
		*/
	//inline value_type normalize()
	//{
	//	value_type norm = glbVector::length();
	//	if (norm>0.0)
	//	{
	//		value_type inv = 1.0f/norm;
	//		x *= inv;
	//		y *= inv;
	//		z *= inv;
	//	}
	//	return( norm );
	//}
public:
	glbDouble x;///<x方向大小
	glbDouble y;///<y方向大小
	glbDouble z;///<z方向大小
}_GlbVECTOR;

/**
  * @brief glbLine 线结构体
  *			 					 
  * @version 1.0
  * @author  ML
  * @date    2017-8-8 14:40
  */
typedef struct _GlbLINE
{
	glbVector P0;
	glbVector P1;
}glbLine;

/**
  * @brief glbSegment 线段结构体
  *			 					 
  * @version 1.0
  * @author  ML
  * @date    2017-8-8 14:40
  */
typedef struct _GlbSEGMENT
{
	glbVector P0;
	glbVector P1;
}glbSegment;
/**
  * @brief glbPlane 平面结构体
  *			 					 
  * @version 1.0
  * @author  ML
  * @date    2017-8-8 14:40
  */
typedef struct _GLBPLANE
{
	glbVector V0;
	glbVector n;
}glbPlane;
/**
  * @brief glbRay 射线结构体
  *			 					 
  * @version 1.0
  * @author  ML
  * @date    2017-8-8 14:40
  */
typedef struct _GLBRAY
{
	glbVector P0;
	glbVector P1;
}glbRay;
	 
/**
  * @brief glbTriangle 三角形结构体
  *			 					 
  * @version 1.0
  * @author  ML
  * @date    2017-8-8 14:40
  */
typedef struct _GLBTRIANGLE
{
	glbVector V0;
	glbVector V1;
	glbVector V2;
}glbTriangle;
/**
  * @brief glbTrack 轨迹结构体
  *			 					 
  * @version 1.0
  * @author  ML
  * @date    2017-8-8 14:40
  */
typedef struct _GLBTRACK
{
	glbVector P0;
	glbVector v;
}glbTrack;
/**
  * @brief glbBlob 二进制大数据结构体				 
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 14:40
  */
typedef struct _GlbBLOB
{
	glbInt32 size; ///<数据个数
	glbByte* pdata;///<数据
}glbBlob;

//引用系统头文件
#include <windows.h>
#include <string>
#include <string.h>
#include <xstring>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
using namespace std;

#define GLBSTRBUFFERSIZE 5120 ///< 字符串缓冲区长度