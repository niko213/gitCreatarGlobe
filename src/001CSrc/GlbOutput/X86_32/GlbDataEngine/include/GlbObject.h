#pragma once
#include <vector>
#include "IGlbDataEngine.h"
using namespace std;
/**
  * @brief 属性对象
  * 
  * @version 1.0
  * @date    2014-03-10 11:21
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbObject :public CGlbReference
{
public:
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
	CGlbObject(IGlbObjectClass*  cls,CGlbFields* fields);
/**
  * @brief 析构函数
  *
  * 没有任何参数及传回值
  */
	~CGlbObject(void);
 /**
  * @brief 初始化函数
  *
  * @param values 对象的字段值数组
  * @return 成功true，失败false
  */
	glbBool Init(GLBVARIANT** values);
public:
 /**
  * @brief 获取对象id
  *
  * @return 成功：对象id，失败：-1
  */
	glbInt32			    GetOid();
 /**
  * @brief 设置对象id
  *
  * @param oid 对象id
  * @return 成功true，失败false
  */
	glbBool				    SetOid(glbInt32 oid);
 /**
  * @brief 获取对象类
  *
  * @return 成功:返回对象类，失败: NULL
  */
	const IGlbObjectClass*	GetClass();
 /**
  * @brief 获取字段信息
  *
  * @return 成功:返回字段信息，失败: NULL
  */
	const CGlbFields*		GetFields();
 /**
  * @brief 获取字段值
  *
  * @param fidx    字段索引
  * @param isdirty 是否修改
  * @param isclean 是否清除修改标志
  * @return        字段值.失败返回NULL.
  */
	const GLBVARIANT*	    GetValue(glbInt32 fidx,glbBool *isdirty=NULL,glbBool isclean=false);
 /**
  * @brief 获取字段值
  *
  * @param fname   字段名
  * @param value   值
  * @param isdirty 是否修改
  * @param isclean 是否清除修改标志
  * @return 成功true，失败false
  */
	const GLBVARIANT*		GetValueByName(const glbWChar* fname,glbBool *isdirty=NULL,glbBool isclean=false);
 /**
  * @brief 设置字段值
  *
  * @param fidx  字段索引
  * @param value 值
  * @return 成功true，失败false
  */
	glbBool				    PutValue(glbInt32 fidx, GLBVARIANT& value);
 /**
  * @brief 设置字段值
  *
  * @param fname 字段名字
  * @param value 值
  * @return 成功true，失败false
  */
	glbBool				    PutValueByName(const glbWChar*  fname,GLBVARIANT& value);
 /**
  * @brief 获取上次执行的错误信息
  *
  * @return 错误信息
  */
	glbWChar*		        GetLastError();
/**
  * @brief 获取大小
  *
  */
	glbUInt32               GetSize();
private:
	typedef struct _FieldValue
	{
		glbBool							isdirty;///<是否修改
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
	glbref_ptr<IGlbObjectClass>	 mpr_class;	 ///<要素类	   
	glbref_ptr<CGlbFields>		 mpr_fields; ///<列信息
	vector<FIELDVALUE>			 mpr_values; ///<字段信息
};