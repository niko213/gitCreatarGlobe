#pragma once
#include "GlbFields.h"
#include "GlbDataEngineExport.h"
/**
  * @brief 索引类
  * 
  * @version 1.0
  * @date    2014-03-10 11:21
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbIndex:public CGlbReference
{
public:
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
	CGlbIndex();
/**
  * @brief 析构函数
  *
  * 没有任何参数及传回值
  */
	~CGlbIndex();
/**
  * @brief 设置索引的名字
  *
  * @param iname 索引名
  * @return 成功true，失败false
  */
	glbBool SetName(const glbWChar* iname);
/**
  * @brief 获取索引名
  *
  * @return 索引名
  */
	const glbWChar* GetName();
/**
  * @brief 添加索引字段
  *
  * @param field 添加的索引字段
  * @return 成功true，失败false
  */
	glbBool AddField(CGlbField* field);
/**
  * @brief 删除索引中某个字段
  *
  * @param fidx 删除的字段位置
  * @return 成功true，失败false
  */
	glbBool RemoveField(glbInt32 fidx);
/**
  * @brief 通过名字删除索引中某个字段
  *
  * @param fname 删除的字段名字
  * @return 成功true，失败false
  */
	glbBool RemoveFieldByName(const glbWChar* fname);
/**
  * @brief 获取索引的字段总数
  *
  * @return 字段总数
  */
	glbInt32 GetFieldCount();
/**
  * @brief 获取索引中某个字段
  *
  * @param fidx 字段在索引中的位置
  * @return 字段
  */
	CGlbField* GetField(glbInt32 fidx);
/**
  * @brief 根据名字获取索引中某个字段
  *
  * @param fname 字段的名字
  * @return 字段
  */
	CGlbField* GetFieldByName(const glbWChar* fname);
/**
  * @brief 设置索引是否为升序
  *
  * @param ascend 升序true，降序false
  * @return 成功true，失败false
  */
	glbBool SetAscending(glbBool ascend);
/**
  * @brief 判断是否为升序
  *
  * @return 升序true，降序false
  */
	glbBool IsAscending();
/**
  * @brief 设置索引是否是唯一索引
  *
  * @param unique 是true，不是false
  * @return 成功true，失败false
  */
	glbBool SetUnique(glbBool unique);
/**
  * @brief 判断是否是唯一索引
  *
  * @return 是true，不是false
  */
	glbBool IsUniqure();
/**
  * @brief 获取上次执行的错误信息
  *
  * @return 错误信息
  */
 const glbWChar* GetLastError();
private:
	CGlbWString		mpr_name;			///< 索引名
	CGlbFields*		mpr_fields;			///< 字段集合
	glbBool			mpr_isAscending;	///< 是否是升序
	glbBool			mpr_isUnique;		///< 是否是唯一索引
};