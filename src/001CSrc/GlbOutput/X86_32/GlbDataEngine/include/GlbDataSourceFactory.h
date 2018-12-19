#pragma once
#include "IGlbDataEngine.h"
#include <map>
using namespace std;

/**
* @brief 数据源工厂
*
* 单实例对象
* 
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API CGlbDataSourceFactory
{
private:
	/**
	* @brief 构造函数
	*
	* 没有任何参数及传回值
	*/
	CGlbDataSourceFactory();
	/**
	* @brief 析构函数
	*
	* 没有任何参数及传回值
	*/
	~CGlbDataSourceFactory();
	static CGlbDataSourceFactory mpr_instance; ///< 数据源工厂实例，静态对象
public:

 /**
  * @brief 数据源工厂实例获取函数
  *
  * @return 返回数据源工厂对象
  */
  static  CGlbDataSourceFactory* GetInstance();
 /**
  * @brief 创建一个数据源
  *
  * @param url          数据源创建的url,不区分大小写
  * @param user         用户名，不区分大小写
  * @param password     密码
  * @param providerName 数据源提供者，默认为文件类型
                        文件类型  : file
						Oracle类型: oracle
  * @return 成功 数据源对象，失败 NULL
  */
  IGlbDataSource* CreateDataSource(const glbWChar* url, const glbWChar* user,
	  const glbWChar* password, const glbWChar*providerName = L"file");
/**
  * @brief 打开一个数据源
  *
  * @param url 数据源创建的url
  * @param user 用户名
  * @param password 密码
  * @param providerName 数据源提供者，默认为文件类型
  * @return 成功 数据源对象，失败 NULL
  */
  IGlbDataSource* OpenDataSource(const glbWChar* url, const glbWChar* user,
	  const glbWChar* password, const glbWChar*providerName = L"file");
  /**
  * @brief 获取数据源的个数
  *
  * @return 返回数据源个数
  */
  glbInt32 GetCount() {return mpr_datasources.size();}
/**
  * @brief 根据下标获取一个数据源
  *
  * @param idx 数据源下标
  * @return 成功 数据源对象，失败 NULL
  */
  IGlbDataSource* GetDataSource(glbInt32 idx);
/**
  * @brief 根据唯一编号获取对应的数据源
  *
  * @param id 数据源唯一编号
  * @return 成功 数据源对象，失败 NULL
  */
  IGlbDataSource* GetDataSourceByID(glbInt32 id);
/**
  * @brief 释放数据源对象，并把map中的对象删除
  *
  * @param ds 数据源
  * @return 成功 true，失败 false
  */
  glbBool ReleaseDataSource(IGlbDataSource* ds);
/**
  * @brief 从工程文件中load数据源
  *
  * @param node 根节点
  * @param prjPath 工程目录
  * @param prjPassword 工程密码
  * @return 成功 true，失败 false
  */
  glbBool Load(void*/*xmlNodePtr*/ node, const glbWChar* prjPath, const glbWChar* prjPassword);
/**
  * @brief 把数据源map按照xml的格式导出到工程文件中
  *
  * @param os 根节点
  * @param prjPath 工程目录
  * @param prjPassword 工程密码
  * @return 成功 true，失败 false
  */
  glbBool Save(void*/*xmlNodePtr*/ os, glbWChar* prjPath, const glbWChar* prjPassword);

/**
  * @brief 获取上次执行的错误信息
  *
  * @return 错误信息
  */
  glbWChar* GetLastError(){return GlbGetLastError();}

private:
	IGlbDataSource* FindDS(const glbWChar* url,const glbWChar* user);
private:
	typedef struct _DatasourceAndPW
	{
		glbref_ptr<IGlbDataSource> datasource;///<数据源
		CGlbWString url;                      ///<访问数据源url
		CGlbWString user;                     ///<用户名
		CGlbWString password;                 ///<密码
	}DatasourceAndPW;
	vector<DatasourceAndPW*> mpr_datasources;///<数据源访问信息集合
	glbInt32 mpr_dsbaseID;                   ///<数据源Id 基数
public:
	map<long/*userid*/,CGlbString/*web token*/> mpr_webtokens;
};
