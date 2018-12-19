/********************************************************************
 * Copyright (c) 2013 超维创想
 * All rights reserved.
 *
 * @file    
 * @brief   
 * @version 1.0
 * @author  GWB
 * @date    2014-03-07 11:28
 ********************************************************************
 */
#pragma once
#pragma warning(disable:4251)

#include <vector>
#include "GlbField.h"
#include "glbref_ptr.h"
#include "GlbExtent.h"
#include "GlbCommTypes.h"
#include "GlbError.h"
#include "GlbDataEngineExport.h"
using namespace std;

#define MAXLEVEL 4 //最大几何level级别，不会到达，最小是0

class IGlbGeometry;

/**
  * @brief 属性集合类
  *
  * 该类继承CGlbReference
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class  GLBDATAENGINE_API CGlbGeometryClass :public CGlbReference
{
public:	
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
	CGlbGeometryClass();
/**
  * @brief 析构函数
  *
  * 没有任何参数及传回值
  */
	~CGlbGeometryClass();
/**
  * @brief 添加一个字段
  * @brief 类型必须是数字类型:
  *        GLB_DATATYPE_BYTE    无符号 8位
  *        GLB_DATATYPE_CHAR    有符号 8位
  *        GLB_DATATYPE_BOOLEAN 布尔
  *        GLB_DATATYPE_INT16   整数，16位
  *        GLB_DATATYPE_INT32   长整数，32位
  *        GLB_DATATYPE_INT64   64位整数
  *        GLB_DATATYPE_FLOAT   单精度浮点数，32字节
  *        GLB_DATATYPE_DOUBLE  浮点数，64位
  *        GLB_DATATYPE_DATE    日期+时间
  *        GLB_DATATYPE_VECTOR  向量【3个double,xyz】
  * @brief 字段必须有缺省值.
  * @brief 字段不容许NULL
  * @brief 字段名称不区分大小写
  *
  * @param fld 将要添加的字段信息
  * @return 成功返回true，失败返回false
  */
	glbBool             AddField( CGlbField* fld);
/**
  * @brief 获取一个字段
  *
  * 根据索引从集合里查找，并返回
  *
  * @param fldindex 要获取字段的索引
  * @return 成功该字段，失败NULL
  */
	CGlbField*          GetField(glbInt32 fldindex);
/**
  * @brief 根据名字获取一个字段
  *
  * 根据名字的匹配查找该字段，并返回
  *
  * @param name 要获取字段的索引
  * @return 成功该字段，失败NULL
  */
	CGlbField*          GetFieldByName(const glbWChar* name);
/**
  * @brief 获取集合中字段总数
  *
  * 返回集合中字段总数
  *
  * @return 字段的个数
  */
	glbInt32            GetFieldCount();
/**
  * @brief 根据名字查找字段在集合的索引
  *
  * 通过名字匹配找到该字段，并返回其下标索引值
  *
  * @param name 要获查找字段的名字
  * @return 查找到返回该字段正确索引，否则返回-1
  */
	glbInt32            FindFieldIndex(const glbWChar* name);
/**
  * @brief 删除指定索引位置的字段
  *
  * 通过索引找到该字段，删除之
  *
  * @param fldindex 要删除的字段索引
  * @return 成功返回true，失败返回false
  */
	glbBool             RemoveField(glbInt32 fldindex);
/**
  * @brief 根据名字删除某个属性字段
  *
  * 通过名字找到该字段，删除之
  *
  * @param name 要删除的字段名
  * @return 成功返回true，失败返回false
  */
	glbBool             RemoveFieldByName(const glbWChar* name);
private:    
	vector<glbref_ptr<CGlbField>>	mpr_vecfields;		///<字段列表	
};

/**
  * @brief 几何对象接口
  *
  * 该类继承CGlbReference
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API IGlbGeometry :public CGlbReference
{
public:
/**
  * @brief 析构函数
  *
  * 没有任何参数及传回值
  */
	virtual ~IGlbGeometry(){};
public:
/**
  * @brief 获取对象几何类型
  *
  * @return 几何类型
  */
	virtual GlbGeometryTypeEnum GetType() =0;
/**
  * @brief 判断几何对象是否为空
  *
  * @return 空，返回true，不为空，返回false
  */
	virtual glbBool             IsEmpty() =0;
/**
  * @brief 获取几何对象外包
  *
  * @return 外包
  */
	virtual const CGlbExtent*   GetExtent()=0;
/**
  * @brief 获取错误信息
  *
  * 最近一次错误信息
  */
	glbWChar*	                GetLastError(){ return GlbGetLastError();}
/**
  * @brief 获取所占用内存大小.
  */
	virtual glbUInt32           GetSize() =0;
/**
  *  @brief 获取坐标纬度:
  *  @brief 2 代表二维，有X,Y
  *  @brief 3 代表三维，有X,Y,Z
  *  @brief 0 代表维数不定,只有CGlbGeometryCollect会返回0
  */
	virtual glbInt32            GetCoordDimension()  =0;
/**
  *  @brief 是否带度量值
  *  @brief true 代表有,false代表无
  *  @brief CGlbGeometryCollect 不能用该返回值确定其子对象是否带度量.
  *                             调用子对象的HasM来判断其是否带度量.
  */
	virtual glbBool             HasM() = 0;
/**
  *  @brief 置空对象
  */
	virtual void                Empty() = 0;
	virtual glbBool             SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls) = 0;
	virtual const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum ctype) = 0;
};

/**
  * @brief 几何对象集合类
  *
  * 该类继承IGlbGeometry
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbGeometryCollect :public IGlbGeometry
{
public:
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
	CGlbGeometryCollect();
/**
  * @brief 析构函数
  *
  * 没有任何参数及传回值
  */
	~CGlbGeometryCollect();
public:
/**
  * @brief 获取对象几何类型
  *
  * @return 几何类型
  */
	GlbGeometryTypeEnum		GetType();
/**
  * @brief 判断几何对象是否为空
  *
  * @return 空，返回true，不为空，返回false
  */
	glbBool					IsEmpty();
/**
  * @brief 获取对象外包
  *
  * @return 外包
  */
	CGlbExtent*				GetExtent();
/**
  * @brief 获取所占用内存大小.
  */
	glbUInt32               GetSize();
/**
  *  @brief 获取坐标纬度:
  *  @brief 2 代表二维，有X,Y
  *  @brief 3 代表三维，有X,Y,Z
  *  @brief 0 代表维数不定,只有CGlbGeometryCollect会返回0
  */
	glbInt32                GetCoordDimension();
/**
  *  @brief 是否带度量值
  *  @brief true 代表有,false代表无
  *  @brief CGlbGeometryCollect 不能用该返回值确定其子对象是否带度量.
                                调用子对象的HasM来判断其是否带度量.
  */
	glbBool                 HasM();
/**
  *  @brief 置空对象
  */
	void                    Empty();
	glbBool             SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls);
	const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum ctype);
public:
/**
  * @brief 添加一个几何对象
  *
  * @param geo 添加的几何对象
  * @return    成功返回true，失败返回false
  */
	glbBool					AddGeometry( IGlbGeometry* geo);
/**
  * @brief 删除一个几何对象
  *
  * @param idx 删除的索引
  * @return 成功返回true，失败返回false
  */
	glbBool					DeleteGeometry(glbInt32 idx);
/**
  * @brief 获取几何对象
  *
  * @param idx 要获取的几何索引位置
  * @param geo 返回几何对象
  * @return 成功返回true，失败返回false
  */
	glbBool					GetGeometry( glbInt32 idx,IGlbGeometry** geo);
/**
  * @brief 获取集合中对象总数
  *
  * @return 返回对象总数
  */
	glbInt32                GetCount();
private:
	vector<glbref_ptr<IGlbGeometry>> mpr_geometries;	///< 几何存储数组
	glbref_ptr<CGlbExtent>           mpr_extent;		///< 外包
};

/**
  * @brief glbArray 数据数组结构体		 
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 14:40
  */
typedef struct  _GlbARRAY
{
	GlbDataTypeEnum vt;		 ///<类型
	glbInt32        dims;     ///<维度
	glbInt32        dimSize;  ///<维度大小
	union
	{
		glbByte*   pBytes;					///< byte数据
		glbChar*   pChars;					///< char数据
		glbBool*   pBools;				    ///< bool数据
		glbInt16*  pInts;					///< int16数据
		glbInt32*  pLongs;					///< int32数据
		glbInt64*  pLLongs;				    ///< int64数据
		glbFloat*  pFloats;					///< float数据
		glbDouble* pDoubles;				///< double数据
        glbWChar** pStrs;				    ///< 宽字符串
		glbDate*   pDates;					///< 日期
		glbref_ptr<IGlbGeometry>** pGeos;	///< 几何数据
		glbVector*   pVectors;				///< double数组
        glbBlob*     pBlobs;				///< blob数据
	};
}glbArray;
/**
  * @brief 自定义变量
  *	@brief 包含基本的数据类型以及复杂类型
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
struct tagGlbVariant
{
	GlbDataTypeEnum vt;		 ///<类型
	glbBool         isnull;  ///<是否是空值
	glbBool         isarray; ///<是否数组
	union
	{
		glbByte   bVal;					///< byte数据
		glbChar   cVal;					///< char数据
		glbBool   blVal;				///< bool数据
		glbInt16  iVal;					///< int16数据
		glbInt32  lVal;					///< int32数据
		glbInt64  llVal;				///< int64数据
		glbFloat  fVal;					///< float数据
		glbDouble dblVal;				///< double数据
        glbWChar  *strVal;				///< 宽字符串
		glbDate   date;					///< 日期
		glbref_ptr<IGlbGeometry>* geo;	///< 几何数据
		glbVector3d   vector;			///< double数组
        glbBlob     blob;				///< blob数据
		glbArray    arrayVal;	        ///< 数组
	};
};
typedef struct tagGlbVariant GLBVARIANT;
typedef struct tagGlbVariant *PGLBVARIANT;

GLBDATAENGINE_API glbBool   glbArrayClean(glbArray& glbArr);
GLBDATAENGINE_API glbBool   glbVariantClean(GLBVARIANT& var);

GLBDATAENGINE_API glbBool   glbArrayCopy(glbArray& destArr,glbArray& srcArr);
GLBDATAENGINE_API glbBool   glbVariantCopy(GLBVARIANT& destVar,GLBVARIANT& srcVar);

GLBDATAENGINE_API glbUInt32 glbArraySize(glbArray& glbArr);
GLBDATAENGINE_API glbUInt32 glbVariantSize(GLBVARIANT& var);
size_t GetDataTypeSize(GlbDataTypeEnum ftype);
/**
  * @brief 设置几何字段值
  *
  *
  * @param dstvalue  几何字段值
  * @param idx       值索引
  * @param count     值数目
  * @param srcvalue  值来源
  * @param ftype     数据类型
  * @return 无
  */
void SetGeoValue(void*           dstvalue,
	             glbInt32        idx,
				 glbInt32        count,
				 const void*     srcvalue,
				 GlbDataTypeEnum ftype);
/**
  * @brief 设置几何字段值
  *
  *
  * @param dstvalue  几何字段值
  * @param idx       值索引
  * @param count     值数目
  * @param srcvalue  值来源
  * @param ftype     数据类型
  * @return 无
  */
void SetGeoValueVariant(void*             dstvalue,
	                    glbInt32          idx,
						glbInt32          count,
						const GLBVARIANT& srcvalue,
						GlbDataTypeEnum   ftype);
/**
  * @brief 拷贝删除几何值
  *
  * srcvalue[0,idx]                ---> dstvalue[0,idx]
  * srcvalue[idx+count,srccount-1] ---> dstvalue[idx+1,..]
  *
  * @param dstvalue  目标几何值
  * @param srcvalue  源几何值
  * @param srccount  源值数目
  * @param idx         
  * @param count     
  * @param ftype     数据类型
  * @return 无
  */
void CopyRemoveGeoValue(void*           dstvalue,
	                    void*           srcvalue,
						glbInt32        srccount,
						glbInt32        idx,
						glbInt32        count,
						GlbDataTypeEnum ftype);
/**
  * @brief 拷贝插入几何值
  *
  * srcvalue [0,  idx]         ----> dstvalue[0,idx]
  * srcvalue [idx,srccount-1]  ----> dstvalue[idx+count]
  * 
  *
  * @param dstvalue    目标几何值
  * @param srcvalue    值来源
  * @param srccount    来源值数目  
  * @param idx         
  * @param count       要插入的值的个数
  * @param ftype       数据类型
  * @return 无
  */
void CopyInsertGeoValue(void*           dstvalue,
	                    void*           srcvalue,
						glbInt32        srccount,
						glbInt32        idx,
						glbInt32        count,
						GlbDataTypeEnum ftype);
/**
  * @brief 获取源数组指定位置的数据
  *
  * 将源数组srcvalue的idx位置的值拷贝到目标变量dstvalue中
  *
  * @param dstvalue 目标变量的首地址
  * @param srcvalue 源数组首地址
  * @param pidx 源数组要取的值的位置索引
  * @param ftype 数据类型
  * @return 无
  */
void GetGeoValue(GLBVARIANT& dstvalue,void* srcvalue,glbInt32 pidx,GlbDataTypeEnum ftype);
/**
  * @brief 获取源数组的值连续拷贝多次到目标数组里
  *
  * 将源数组srcvalue的值，拷贝count次到目标数组dstvalue中
  *
  * @param dstvalue 目标数组的首地址
  * @param srcvalue 源数组首地址
  * @param count 拷贝的次数
  * @param ftype 数据类型
  * @return 无
  */
void GetGeoValues(void** dstvalue,void* srcvalue,glbInt32 count,GlbDataTypeEnum ftype);