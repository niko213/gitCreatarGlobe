#pragma once
#pragma warning(disable:4091)
#include "GlbIndex.h"
#include "GlbDataEngineExport.h"
/**
  * @brief 索引类
  * 
  * @version 1.0
  * @date    2014-03-10 11:21
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbIndexes:public CGlbReference
{
public:
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
CGlbIndexes();
/**
  * @brief 析构函数
  *
  * 没有任何参数及传回值
  */
~CGlbIndexes();
public:
/**
  * @brief 获取索引的总个数
  *
  * @return 成功true，失败false
  */
 glbInt32           GetIndexesCount();
/**
  * @brief 获取某个索引
  *
  * @param fidx 索引的位置
  * @return 取得的索引
  */
 CGlbIndex*			GetIndex(glbInt32 fidx);
/**
  * @brief 根据索引名获取某个索引
  *
  * @param iname 索引的名字
  * @return 取得的索引
  */
 CGlbIndex*			GetIndexByName(const glbWChar* iname);
/**
  * @brief 往几何里添加一个索引
  *
  * @param index 添加的索引的位置
  * @return 成功true，失败false
  */
 glbBool            AddIndex(CGlbIndex* index);
/**
  * @brief 从集合里删除一个索引
  *
  * @param fidx 删除索引的位置
  * @return 成功true，失败false
  */
 glbBool			RemoveIndex(glbInt32 fidx);
/**
  * @brief 根据名字从集合里删除一个索引
  *
  * @param name 删除索引的名字
  * @return 成功true，失败false
  */
 glbBool			RemoveIndexByName(const glbWChar* name);
/**
  * @brief 获取上次执行的错误信息
  *
  * @return 错误信息
  */
 const glbWChar*	GetLastError();

private:
 vector<glbref_ptr<CGlbIndex>> mpr_indexes; ///< 索引数组
};