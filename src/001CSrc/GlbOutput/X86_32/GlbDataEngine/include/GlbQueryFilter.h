#pragma once
#include "IGlbGeometry.h"
/**
* @brief 查询器类
* 
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API CGlbQueryFilter: public CGlbReference
{
public:
	/**
	* @brief 构造函数
	*
	* 没有任何参数及传回值
	*/
	CGlbQueryFilter();
	/**
	* @brief 析构函数
	*
	* 没有任何参数及传回值
	*/
	~CGlbQueryFilter();
	/**
	* @brief 设置where子句
	*
	* @param wherestr 要设置的where子句
	* @return 成功:true，失败:false
	*/
	glbBool SetWhereStr(const glbWChar* wherestr );
	/**
	* @brief 获取where子句
	*
	* @return 成功:where子句，失败:NULL
	*/
	const glbWChar* GetWhereStr();
	/**
	* @brief 设置查询语句后缀关键字
	*
	* @param clause 要设置的后缀关键字
	* @return 成功:true，失败:false
	*/
	glbBool SetPostfixClause(const glbWChar* clause);
	/**
	* @brief 获取查询语句后缀关键字
	*
	* @return 成功:返回查询语句后缀关键字，失败:NULL
	*/
	const glbWChar* GetPostfixClause();
	/**
	* @brief 设置查询语句前缀关键字
	*
	* @param clause 要设置的前缀关键字
	* @return 成功:true，失败:false
	*/
	glbBool SetPrefixClause(const glbWChar* clause);
	/**
	* @brief 获取查询语句前缀关键字
	*
	* @return 成功:返回查询语句前缀关键字，失败:NULL
	*/
	const glbWChar* GetPrefixClause();
	/**
	* @brief 设置查询的字段
	*
	* @param fields 要设置的查询字段
	* @return 成功:true，失败:false
	*/
	glbBool SetFields(const glbWChar* fields);
	/**
	* @brief 获取查询的字段
	*
	* @return 成功:返回查询的字段，失败:NULL
	*/
	const  glbWChar* GetFields();
	/**
	* @brief 设置空间查询器
	*
	* @param geo 用来做空间查询的几何对象
	* @param spatialOp 查询的操作类型，包括外包查询，空间相交等查询
	* @return 成功:true，失败:false
	*/
	glbBool   PutSpatialFilter(IGlbGeometry *geo, glbInt32 spatialOp);
	/**
	* @brief 获取用来空间查询的几何对象
	*
	* @param spatialOp 操作类型
	* @return 成功:返回几何对象，失败:NULL
	*/
	IGlbGeometry* GetSpatialFilter(glbInt32* spatialOp);
	/**
	* @brief 查询结果再次过滤，设置查看结果集的起始行
	*
	* @param startrow 查看的起始行,默认设置为-1
	* @param endrow 查看终止行,默认设置为-1
	* @return 成功:true，失败:false
	*/
	glbBool SetStartEndRow(glbInt32 startrow = -1,glbInt32 endrow = -1);
	/**
	* @brief 获取查看结果集的起始行
	*
	* @param startrow 返回查看的起始行
	* @param endrow 返回查看的终止行
	* @return 成功:true，失败:false
	*/
	glbBool GetStartEndRow(glbInt32* startrow,glbInt32* endrow);
	/**
	* @brief 获取最近一次执行的错误信息
	*
	* @return 错误信息
	*/
	glbWChar* GetLastError();
	/**
	* @brief 设置相机方向  （焦点 - 相机位置）
	*
	* @param xdir,ydir,zdir 相机方向
	* @return 成功:true，失败:false
	*/	
	glbBool SetCameraDir(glbDouble xdir, glbDouble ydir, glbDouble zdir);
	/**
	* @brief 获取相机方向  （焦点 - 相机位置）
	*
	* @param xdir,ydir,zdir 相机方向
	* @return 成功:true，失败:false
	*/
	glbBool GetCameraDir(glbDouble* xdir, glbDouble* ydir, glbDouble* zdir);
	/**
	* @brief 设置是否需要查询Geometry字段
	*
	* @param isQGeo - true(默认) ： 查询geometry字段， false : 不查询geomtry字段
	* @return 成功:true，失败:false
	* @note 当geometry字段数据量很大（如tin），但实际情况中又需要使用geometry时可用此接口
	*/	
	glbBool SetIsQueryGeometry(glbBool isQGeo);
	/**
	* @brief 获取是否需要查询Geometry字段
	*
	* @param true： 查询geometry字段， false : 不查询geomtry字段
	* @return 成功:true，失败:false
	*/
	glbBool IsQueryGeometry();
private:
	CGlbWString						mpr_strWhere;			///< 查询的where子句
	CGlbWString						mpr_strPostfixClause;	///< 查询的后缀语句
	CGlbWString						mpr_strPrefixClause;	///< 查询的前缀语句
	CGlbWString						mpr_strFieldsStr;		///< 查询的字段集
	glbref_ptr<IGlbGeometry>		mpr_geo;				///< 空间查询的几何对象
	glbInt32						mpr_spatialOp;			///< 空间查询的操作类型，外包查询，相交查询等
	glbInt32						mpr_startrow;			///< 查看的起始行
	glbInt32						mpr_endrow;				///< 查看的终止行
	std::vector<glbDouble>			mpr_cameraDir;			///< 相机方向
	glbBool							mpr_isQueryGeo;			///< 是否查询geo字段
};