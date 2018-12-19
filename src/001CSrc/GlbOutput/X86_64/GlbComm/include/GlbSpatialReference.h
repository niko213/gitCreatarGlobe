/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbSpatialReference.h
  * @brief   空间参考系头文件
  *
  *
  * @version 1.0
  * @author  LJH
  * @date    2014-3-13 14:40
*********************************************************************/
#pragma once
#include "GlbReference.h"
#include "GlbCommTypes.h"
class GLBCOMM_API CGlbSpatialReference : public CGlbReference
{
	friend class CGlbCoordTrans;
protected:
	//explicit 抑制隐式转换
	explicit CGlbSpatialReference(void* handle);
public:
	
	~CGlbSpatialReference(void);
public:
	/**
		* @brief 根据WKT格式的空间参考字符串，创建空间参考系
		*
		* @param wkt WKT格式的空间参考字符串.		
		* @return 失败返回NULL
		*/
	static CGlbSpatialReference* CreateFromWKT(const glbWChar* wkt);
	/**
		* @brief 根据Proj4格式的空间参考字符串，创建空间参考系
		*
		* @param proj4 Proj4格式的空间参考字符串.		
		* @return 失败返回NULL
		*/
	static CGlbSpatialReference* CreateFromProj4(const glbWChar* proj4);
	/**
		* @brief 根据Esri格式的空间参考字符串，创建空间参考系
		*
		* @param esri Esri格式的空间参考字符串.		
		* @return 失败返回NULL
		*/
	static CGlbSpatialReference* CreateFromEsriFile(const glbWChar* esri);
	/**
		* @brief 根据国际标准编码，创建空间参考系
		*
		* @param epsgCode 空间参考的国际编码.		
		* @return 失败返回NULL
		*/
	static CGlbSpatialReference* CreateFromEPSG (glbInt32 epsgCode);
	/**
		* @brief 根据广泛使用约定俗成的名称，创建空间参考系
		*
		* @param wellknown 空间参考的名称.		
		* @return 失败返回NULL
		*/
	static CGlbSpatialReference* CreateFromWellKnown(const glbWChar* wellknown);
	/**
		* @brief 根据Ozi规范，创建空间参考系
		*
		* @param pszDatum 椭球提名
		* @param pszProj  投影名称
		* @param pszProjParms 投影参数
		* @return 失败返回NULL
		*/
	static CGlbSpatialReference* CreateFromOzi(const glbWChar *pszDatum,
		                                       const glbWChar *pszProj,
											   const glbWChar *pszProjParms
											   );
	/**
		* @brief 空间参考输出为WKT格式
		*		
		* @return 失败返回NULL
		*/
	glbWChar* ExportToWKT();
	/**
	 * @brief 判断是否是地理坐标系:球面坐标系，参考平面地是 椭球面。坐标单位:经纬度
	 * @brief 经度是通过该点的大地子午面与起始大地子午面之间的夹角
	 * @brief 纬度是通过该点的法线与赤道面的夹角
	 * @brief 高是地面点沿法线到参考椭球面的距离
	 * @return 是返回true，否返回false
	 */
	glbBool IsGeographic();
	/**
	 * @brief 判断是否是地心坐标系:以地球质心为原点建立的空间直角坐标系，
	 * @brief 或以球心与地球质心重合的地球椭球面为基准面所建立的大地坐标系.
	 * @brief 分为：地心空间直角坐标系，地心大地坐标系
	 * @brief WGS84
	 *
	 * @return 是返回true，否返回false
	 *
	 */
	glbBool IsGeocentric();
	/**
	 * @brief 判断是否是投影坐标系
	 *
	 * @return 是返回true，否返回false
	 *
	 */
	glbBool IsProjected();
	/**
	 * @brief 判断是否是高程坐标系
	 *
	 * @return 是返回true，否返回false
	 *
	 */
	glbBool IsVertical();
	/**
	  * @brief 获取大地基准面名称
	  *
	  * @return 没有返回NULL
	  */
	glbWChar* GetDatum();
	/**
	  * @brief 获取参考托球体名称
	  *
	  * @return 没有返回NULL
	  */
	glbWChar* GetSpheroid();
	/**
	  * @brief 获取参考托球体长轴半径
	  *
	  * @return 长轴半径,失败返回0.0;
	  */
	glbDouble GetSemiMajor();
	/**
	  * @brief 获取参考托球体短轴半径
	  *
	  * @return 短轴半径,失败返回0.0;
	  */
	glbDouble GetSemiMinor();
	/**
	  * @brief 判断是否是相同的空间参考
	  *
	  * @return 相同返回true，不同返回false;
	  */
	glbBool IsSame(CGlbSpatialReference* other);

private:
	void* mpr_handle;
};

class GLBCOMM_API CGlbCoordTrans : public CGlbReference
{
public:	
	CGlbCoordTrans();
	~CGlbCoordTrans();
	/**
	  * @brief 初始化
	  *
	  * @param poSrc 源空间参考
	  * @param poTarget 目标空间参考
	  *
	  * @return 参数poSrc或者poTarget是NULL，返回false
	  */
	glbBool Initialize(CGlbSpatialReference* poSrc,CGlbSpatialReference* poTarget);
	/**
	  * @brief 把坐标从源空间参考 转换 为目标空间参考下的值
	  *
	  * @param x 源空间参考X轴坐标
	  * @param y 源空间参考Y轴坐标
	  * @param z 源空间参考Z轴坐标
	  * @param outx 转换后目标空间参考X轴坐标
	  * @param outy 转换后目标空间参考Y轴坐标
	  * @param outz 转换后目标空间参考Z轴坐标
	  *
	  * @return 成功返回true，没有初始化等失败返回false
	  */
	glbBool Transform(glbDouble x, glbDouble y, glbDouble z,glbDouble *outx, glbDouble *outy, glbDouble *outz);
private:
	void* mpr_handle;
};
const glbDouble WGS_84_RADIUS_EQUATOR = 6378137.0;
const glbDouble WGS_84_RADIUS_POLAR = 6356752.3142;
const glbDouble PI   = 3.14159265358979323846;
class GLBCOMM_API CGlbEllipsoidModel:public CGlbReference
{
public:
	/**
	  * @brief 构造函数
	  *
	  * @param radiusEquator 椭球体长半径(赤道半径)
	  * @param radiusPolar   椭球体短半径
	  */
	CGlbEllipsoidModel(glbDouble radiusEquator=WGS_84_RADIUS_EQUATOR,
		               glbDouble radiusPolar=WGS_84_RADIUS_POLAR);
	/**
	  *  @brief 把经纬高转换为笛卡尔
	  * 
	  *  @param longitude 经度,单位弧度
	  *  @param latitude 纬度,单位弧度	  
	  *  @param height 高度,单位弧度
	  *  @param X 转换后的X轴坐标值
	  *  @param Y 转换后的Y轴坐标值
	  *  @param Z 转换后的Z轴坐标值
	  */
	void ConvertLatLongHeightToXYZ(glbDouble latitude,glbDouble longitude, glbDouble height,
                                   glbDouble& X, glbDouble& Y, glbDouble& Z) const;
	/**
	  *  @brief 把笛卡尔转换为经纬高
	  * 
	  *  @param X X轴坐标值
	  *  @param Y Y轴坐标值
	  *  @param Z Z轴坐标值
	  *  @param longitude 转换后的经度,单位弧度
	  *  @param latitude  转换后的纬度,单位弧度	  
	  *  @param height    转换后的高度,单位弧度	  
	  */
	void ConvertXYZToLatLongHeight(glbDouble X, glbDouble Y, glbDouble Z,
                                   glbDouble& longitude, glbDouble& latitude, glbDouble& height) const;
	/*
		@brief 度转弧度
	*/
	glbDouble DegreesToRadians(glbDouble angle);
	/*
	    @brief 弧度转度
	*/
	glbDouble RadiansToDegrees(glbDouble angle);
private:
	glbDouble mpr_radiusEquator;
    glbDouble mpr_radiusPolar;
	glbDouble mpr_eccentricitySquared;
};


