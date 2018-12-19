/********************************************************************
 * Copyright (c) 2013 超维创想
 * All rights reserved.
 *
 * @file    
 * @brief   
 * @version 1.0
 * @author  GWB
 * @date    2014-03-10 11:21
 ********************************************************************
 */
#pragma once
#include "GlbWString.h"
#include "GlbDataEngineExport.h"
#include "GlbDataEngineType.h"
#include "GlbReference.h"
#pragma warning(disable:4251)
/**
  * @brief 字段类
  *
  * 包含字段的信息，可获取，设置修改
  * 
  * @version 1.0
  * @date    2014-03-10 11:21
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbField :public CGlbReference
{
public:
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
 CGlbField();
/**
  * @brief 析构函数
  *
  * 没有任何参数及传回值
  */
 ~CGlbField();
private:
 CGlbWString             mpr_name;					///<字段名
 CGlbWString             mpr_alias; 				///<字段别名
 GlbDataTypeEnum		 mpr_type;					///<字段类型
 GlbGeometryTypeEnum	 mpr_geotype;				///<字段几何类型
 glbInt32		         mpr_length;				///<字段长度
 CGlbWString             mpr_unit;  				///<单位
 CGlbWString             mpr_userid;				///<用户名
 CGlbWString             mpr_password;				///<密码
 glbBool                 mpr_isrequired;			///<是否必须
 glbBool                 mpr_iscannull;				///<是否能为空
 CGlbWString			 mpr_default;        		///<默认值
 glbInt32                mpr_id;					///<id值
public:

/**
  * @brief 获取字段id值
  *
  * @return 字段id
  */
 const glbInt32          GetID();
/**
  * @brief 设置字段id
  *
  * 字符串到该字符串
  *
  * @param id 设置的id值
  * @return 成功true，失败false
  */
 glbBool                 SetID(glbInt32 id);
/**
  * @brief 获取字段名,不区分大小写
  *
  * @return 字段名
  */
 const glbWChar*         GetName();
/**
  * @brief 设置字段名,不区分大小写
  *
  * @param name 用来设置字段名
  * @return 成功true，失败false
  */
 glbBool                 SetName(const glbWChar* name);
/**
  * @brief 获取字段别名,不区分大小写
  *
  * @return 字段别名
  */
 const glbWChar*         GetAlias();
/**
  * @brief 设置字段别名
  *
  * @param aliasname 用来设置的字段别名
  * @return 成功true，失败false
  */
 glbBool                 SetAlias(const glbWChar* aliasname);
/**
  * @brief 获取字段类型
  *
  * @return 字段类型
  */
 GlbDataTypeEnum         GetType();
/**
  * @brief 设置字段的数据类型
  *
  * @param datatype 字段类型
  * @return 成功true，失败false
  */
 glbBool                 SetType(GlbDataTypeEnum datatype);
/**
  * @brief 获取GLB_DATATYPE_STRING字符串字段的长度
  *
  * @return 字段的长度
  */
 glbInt32                GetLength();
/**
  * @brief 设置GLB_DATATYPE_STRING字符串字段的长度
  *
  * @param length    长度
  * @return 成功true，失败false
  */
 glbBool                 SetLength(const glbInt32 length);
/**
  * @brief 获取GLB_DATATYPE_GEOMETRY类型字段的几何类型
  *
  * @return 几何类型
  */
 GlbGeometryTypeEnum     GetGeoType();
/**
  * @brief 设置GLB_DATATYPE_GEOMETRY类型字段的几何类型
  *
  * @param geoType 几何类型
  * @return 成功true，失败false
  */
 glbBool                 SetGeoType(GlbGeometryTypeEnum geoType);
/**
  * @brief 获取字段的单位
  *
  * @return 单位
  */
 const glbWChar*         GetUnit();
/**
  * @brief 设置字段的单位
  *
  * @param unit 单位
  * @return 成功true，失败false
  */
 glbBool                 SetUnit(const glbWChar* unit);
/**
  * @brief 获取用户id
  *
  * @return 用户id
  */
 const glbWChar*         GetUserID();
/**
  * @brief 设置用户id
  *
  * @param userid 用户id
  * @return 成功true，失败false
  */
 glbBool                 SetUserID(const glbWChar* userid);
/**
  * @brief 获取用户密码
  *
  * @return 用户密码
  */
 const glbWChar*         GetPassWord();
/**
  * @brief 设置用户密码
  *
  * @param pw 用户密码
  * @return 成功true，失败false
  */
 glbBool                 SetPassWord(const glbWChar* pw);
/**
  * @brief 判断该字段是否是系统字段,系统字段不能删除.
  *
  * @return 是true，不是false
  */
 glbBool                 IsRequired();
/**
  * @brief 设置字段是否是系统字段
  *
  * @param bisrequired 设置值，true或false
  * @return 成功true，失败false
  */
 glbBool                 SetRequired(glbBool bisrequired);
/**
  * @brief 判断该字段的值是否可以是NULL.
  *
  * @return 是true，不是false
  */
 glbBool                 IsCanNull();
/**
  * @brief 设置字段是否可以为空
  * @brief GLB_DATATYPE_GEOMETRY,GLB_DATATYPE_BLOB 字段必须容许NULL.
  *
  * @param bisnull 设置值，true或false
  * @return 成功true，失败false
  */
 glbBool                 SetCanNull(glbBool bisnull);
/**
  * @brief 设置默认值
  *        Geometry、Blob默认值为NULL,不需要设置
           GLB_DATATYPE_BYTE
		   GLB_DATATYPE_CHAR		     
		   GLB_DATATYPE_INT16    
           GLB_DATATYPE_INT32    
           GLB_DATATYPE_INT64    
           GLB_DATATYPE_FLOAT    
           GLB_DATATYPE_DOUBLE
		       以上类型,默认值转换为字符串存储
		   GLB_DATATYPE_BOOLEAN
		       true 转换为 "1",false 转换为 "0"
           GLB_DATATYPE_STRING
		   GLB_DATATYPE_DATE
		       日期(时间)转换为Double再转换为字符串存储
		   GLB_DATATYPE_VECTOR
		       转换为"x,y,z"格式的字符串存储
  * @param df 默认值
  * @return 成功true，失败false
  */
 glbBool                 SetDefault(const glbWChar* df);
/**
  * @brief 获取字段默认值
  *
  *        Geometry、Blob默认值为NULL,不需要设置
           GLB_DATATYPE_BYTE
		   GLB_DATATYPE_CHAR		     
		   GLB_DATATYPE_INT16    
           GLB_DATATYPE_INT32    
           GLB_DATATYPE_INT64    
           GLB_DATATYPE_FLOAT    
           GLB_DATATYPE_DOUBLE
		       以上类型,默认值转换为字符串存储
		   GLB_DATATYPE_BOOLEAN
		       true 转换为 "1",false 转换为 "0"
           GLB_DATATYPE_STRING
		   GLB_DATATYPE_DATE
		       日期(时间)转换为Double再转换为字符串存储
		   GLB_DATATYPE_VECTOR
		       转换为"x,y,z"格式的字符串存储
  * @return 字段默认值
  */
 const glbWChar*         GetDefault();
/**
  * @brief 获取上次执行的错误信息
  *
  * @return 错误信息
  */
 glbWChar*               GetLastError();
};