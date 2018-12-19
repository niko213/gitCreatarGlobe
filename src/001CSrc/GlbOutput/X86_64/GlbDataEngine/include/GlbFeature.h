#pragma once
#include <vector>
#include <map>
#include "IGlbDataEngine.h"
using namespace std;
/**
  * @brief 要素对象
  *
  * 一个要素对象，包含若干普通字段和几何字段
  * 
  * @version 1.0
  * @date    2014-03-10 11:21
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbFeature :public CGlbReference
{
public:
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
	CGlbFeature( IGlbFeatureClass* cls,CGlbFields* fields);
/**
  * @brief 析构函数
  *
  * 没有任何参数及传回值
  */
	~CGlbFeature(void);
 /**
  * @brief 初始化要素对象
  *
  * @param values 字段值
  * @return 成功true，失败false
  */
	virtual glbBool     Init(GLBVARIANT** values,CGlbExtent* ext);
public:
/**
  * @brief 获取对象id
  *
  * @return 成功：对象id，失败：-1
  */
	glbInt32                  GetOid();
/**
  * @brief 设置对象的oid
  *
  * @param oid 设置的oid值
  * @return 成功true，失败false
  */
	glbBool				      SetOid(glbInt32 oid);
/**
  * @brief 获取对象的FeatureClass
  *
  * @return 成功:返回对象FeatureClass，失败: NULL
  */
	const IGlbFeatureClass*   GetClass();
/**
  * @brief 获取对象的所有字段信息
  *
  * @return 成功:返回字段信息，失败: NULL
  */
	const CGlbFields*         GetFields();
/**
  * @brief 获取对象的空间坐标信息
  *
  * @return 成功:返回空间坐标信息，失败: NULL
  */
	const glbWChar*           GetSRS();
/**
  * @brief 获取对象主几何的外包
  *
  * @return 成功:返回外包，失败: NULL
  */
	const CGlbExtent*         GetExtent();
/**
  * @brief 获取对象字段值
  *
  * @param fidx    字段位置索引
  * @param value   获取的值
  * @param isdirty 值是否被修改过
  * @param isclean 是否清除修改标志
  * @return 成功true，失败false
  */
	const GLBVARIANT*         GetValue(glbInt32 fidx,glbBool *isdirty=NULL,glbBool isclean=false);
/**
  * @brief 获取对象字段值
  *
  * @param fname 字段名
  * @param value 获取的值
  * @param isdirty 值是否被修改过
  * @param isclean 是否清除修改标志
  * @return 成功true，失败false
  */
	const GLBVARIANT*         GetValueByName( const glbWChar* fname,glbBool *isdirty=NULL,glbBool isclean=false);
/**
  * @brief 设置对象字段值
  *
  * @param fidx  字段位置索引
  * @param value 设置的值
  * @return 成功true，失败false
  */
	glbBool                   PutValue(glbInt32 fidx, GLBVARIANT& value);
/**
  * @brief 设置对象字段值
  *
  * @param fname 字段名字
  * @param value 设置的值
  * @return 成功true，失败false
  */
	glbBool                   PutValueByName(const glbWChar* fname, GLBVARIANT& value);
/**
  * @brief 获取上次执行的错误信息
  *
  * @return 错误信息
  */
	glbWChar*                 GetLastError();
/**
  * @brief 获取一级的几何对象
  *
  * @param geo   几何对象
  * @param level 级别
  * @return 成功true，失败false
  */
	glbBool	                  GetGeometry(IGlbGeometry** geo, glbUInt32 level=0,glbBool *isdirty=NULL,glbBool isclean=false,glbBool needquery=true);
/**
  * @brief 设置几何数据
  *
  * @param geo   要设置的几何数据
  * @param level 要设置的数据的级别
  * @return 成功true，失败false
  */
	glbBool                   PutGeometry(IGlbGeometry* geo, glbUInt32 level,glbBool setdirty=true);
private:
	typedef struct _FieldValue
	{
		glbBool							isdirty;///<值是否修改
		GLBVARIANT*						value;	///<几何变量
		_FieldValue()
		{
			isdirty = false;
			value   = NULL;
		}
		~_FieldValue()
		{
			if(value)
			{
				glbVariantClean(*value);
				delete value;
			}
		}
	}FIELDVALUE;
	typedef struct _GeometryValue
	{
		glbBool                         isdirty;///<值是否修改
		glbref_ptr<IGlbGeometry>        geo;	///<几何
		_GeometryValue()
		{
			isdirty = false;
			geo     = NULL;
		}
	}GEOMETRYVALUE;

	glbref_ptr<IGlbFeatureClass>		 mpr_class;	
	glbref_ptr<CGlbFields>               mpr_fields;
	vector<FIELDVALUE>					 mpr_values;
protected://继承者可以访问.	
	glbref_ptr<CGlbExtent>				  mpr_extent;
	map<glbUInt32/*级别*/, GEOMETRYVALUE> mpr_geos;	 
};