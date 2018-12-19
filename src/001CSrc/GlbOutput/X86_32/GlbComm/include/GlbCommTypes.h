/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbCommType.h
  * @brief   ���Ͷ���
  *
  * �����������GlbAltitudeModeEnum��GlbTextAlignmentEnum��GlbTextAxisAlignmentEnum
  * GlbFontSizeModeEnum��GlbLogTypeEnum��GlbCompressTypeEnumö�����ͣ�
  * glbVector��glbBlob��glbArray�ṹ�����ͣ������������������¶��塣
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 14:40
*********************************************************************/
#pragma once
/**
  * @brief GlbLogTypeEnum ��־����ö��
  *
  * ����2����־���ͣ�һ����ͨ������Ϣ��һ���Ǵ�����Ϣ				 
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40
  */
enum GlbLogTypeEnum
{
	GLB_LOGTYPE_INFO=0,///<��־��ͨ��Ϣ
	GLB_LOGTYPE_ERR =1 ///<��־������Ϣ
};
/**
  * @brief GlbCompressTypeEnumѹ������ö��
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-12 16:40
  */
enum GlbCompressTypeEnum
{
	GLB_COMPRESSTYPE_NO   = 0, ///<��ѹ��
	GLB_COMPRESSTYPE_LZW  = 1, ///<lzwѹ��
	GLB_COMPRESSTYPE_ZLIB = 2, ///<zlibѹ��
	GLB_COMPRESSTYPE_JPEG = 3 ///<jpegѹ��	
};
/**
  * @brief 32,64λ����ע������
  *
  * 1.��������     LP32    LP64
  * char      8       8
  * short     16      16
  * int       32      32 
  * longlong  64      64
  * long      32      64
  * ָ��      32       64
  * time_t    32       64
  * size_t    32       64
  * 2.ȡ����λ����ʹ��sizeof��������ֱ��д����
  * ��sizeof(long)����32
  * 3.%x��ʮ������ int ��ʽ��printf ���η��� 64 λ Windows ����ϵͳ�в���Ԥ�ڵ�������������ֻ�Դ��ݸ�����ֵ��ǰ 32 λ���в�����
  * Windows 32 λ����ϵͳʹ�� %I32x ��ʾ������
  * Windows 64 λ����ϵͳʹ�� %I64x ��ʾ������
  * %p��ָ���ʮ�����Ƹ�ʽ���� 64 λ Windows ����ϵͳ�а�Ԥ�ڵ�����������
  * 4.#ifdef _WIN64
  * 
  * 5. ϵͳAPI 32/64 λ�����汾			 
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 14:40
  */
typedef __int8  glbInt8  ;///<8λ�з�������
typedef __int16 glbInt16 ;///<16λ�з�������
typedef __int32 glbInt32 ;///<32λ�з�������
typedef __int64 glbInt64 ;///<64λ�з�������
typedef bool    glbBool;  ///<����
typedef char    glbChar;  ///<�ַ�
typedef wchar_t glbWChar; ///<���ַ�
typedef float   glbFloat; ///<�����ȸ�����
typedef double  glbDouble;///<˫���ȸ�����
typedef glbInt64  glbDate;  ///<ʱ�䣿�ǲ�����glbInt64
typedef unsigned __int8  glbUInt8; ///<8λ�޷�������
typedef unsigned __int16 glbUInt16;///<16λ�޷�������
typedef unsigned __int32 glbUInt32;///<32λ�޷�������
typedef unsigned __int64 glbUInt64;///<64λ�޷�������
typedef unsigned __int8  glbByte;  ///<�ֽ�

/**
  * @brief glbVector �����ṹ��
  *			 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 14:40
  */
typedef struct _GLBVECTOR3D
{
	glbDouble x;///<x�����С
	glbDouble y;///<y�����С
	glbDouble z;///<z�����С
}glbVector3d;

/**
  * @brief glbVector �����ṹ��
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
	glbDouble x;///<x�����С
	glbDouble y;///<y�����С
	glbDouble z;///<z�����С
}_GlbVECTOR;

/**
  * @brief glbLine �߽ṹ��
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
  * @brief glbSegment �߶νṹ��
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
  * @brief glbPlane ƽ��ṹ��
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
  * @brief glbRay ���߽ṹ��
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
  * @brief glbTriangle �����νṹ��
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
  * @brief glbTrack �켣�ṹ��
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
  * @brief glbBlob �����ƴ����ݽṹ��				 
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 14:40
  */
typedef struct _GlbBLOB
{
	glbInt32 size; ///<���ݸ���
	glbByte* pdata;///<����
}glbBlob;

//����ϵͳͷ�ļ�
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

#define GLBSTRBUFFERSIZE 5120 ///< �ַ�������������