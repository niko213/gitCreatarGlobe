#pragma once
#include <vector>
#include "GlbField.h"
#include "GlbDataEngineExport.h"
#include "glbref_ptr.h"
using namespace std;
/**
  * @brief 字段集合类
  * 
  * @version 1.0
  * @date    2014-03-10 11:21
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbFields:public CGlbReference
{
public:
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
	CGlbFields();
/**
  * @brief 析构函数
  *
  * 没有任何参数及传回值
  */
	~CGlbFields();
public:
/**
  * @brief 获取字段个数值
  *
  * @return 字段总个数
  */
	glbInt32 GetFieldCount();
/**
  * @brief 获取某个字段
  *
  * @param fidx 字段的位置
  * @return 字段
  */
	CGlbField* GetField(glbInt32 fidx);
/**
  * @brief 根据字段名字获取某个字段
  *
  * @param iname 字段名
  * @return 字段
  */
	CGlbField* GetFieldByName(const glbWChar* iname);
/**
  * @brief 根据字段名字获取某个字段下标
  *
  * @param fname 字段名
  * @return 该字段下标
  */
	glbInt32 FindFieldIndex(const glbWChar* fname);
/**
  * @brief 添加一个字段
  *
  * @param field 要添加的字段
  * @return 成功true，失败false
  */
	glbBool AddField(CGlbField* field);
/**
  * @brief 删除一个字段
  *
  * @param fidx 要删除的字段的下标
  * @return 成功true，失败false
  */
	glbBool RemoveField(glbInt32 fidx);
/**
  * @brief 根据名字删除一个字段
  *
  * @param iname 要删除的字段的名字
  * @return 成功true，失败false
  */
	glbBool RemoveFieldByName( const glbWChar* iname);
/**
  * @brief 获取上次执行的错误信息
  *
  * @return 错误信息
  */
	glbWChar* GetLastError();

private:
	vector<glbref_ptr<CGlbField>> mpr_fields; ///<字段集合数组
};