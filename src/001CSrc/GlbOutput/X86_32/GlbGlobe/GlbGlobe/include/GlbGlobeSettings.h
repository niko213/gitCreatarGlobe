#pragma once
#include <vector>
#include <osg/Vec3d>
#include <osg/CoordinateSystemNode>
#include "GlbGlobeExport.h"
/********************************************************************
  * Copyright (c) 2014 北京超维创想信息技术有限公司
  * All rights reserved.
  *  
  *
  * 这个档案定义配置信息,
  *
  * @version 1.0
  * @author  马林
  * @date    2014-5-5 10:40
*********************************************************************/
//星空绘制顺序
#ifndef GlB_STARS_RENDERBIN
#define GlB_STARS_RENDERBIN       -11000
#endif
//太阳绘制顺序
#ifndef GLB_SUN_RENDERBIN
//#define GLB_SUN_RENDERBIN         -10900
#define GLB_SUN_RENDERBIN         -10500
#endif
//月亮绘制顺序
#ifndef GLB_MOON_RENDERBIN
//#define GLB_MOON_RENDERBIN        -10800
#define GLB_MOON_RENDERBIN        -10400
#endif
//大气绘制顺序
#ifndef GLB_ATMOSPHERE_RENDERBIN
#define GLB_ATMOSPHERE_RENDERBIN  -10700
#endif
//天空盒绘制顺序
#ifndef  GLB_SKYBOX_RENDERBIN
#define  GLB_SKYBOX_RENDERBIN     -10600
#endif
//遮挡大气用基础球绘制顺序
#ifndef  GLB_BASEGLOBE_RENDERBIN
#define  GLB_BASEGLOBE_RENDERBIN     -10001
#endif
#ifndef  GLB_BASEGLOBE_RENDERBIN
#define  GLB_BASEGLOBE_RENDERBIN     -10001
#endif

//配置信息变化监听接口
//#define LogoFile_Setting   L"LogoFile"
//#define SkyTexFile_Setting L"SkyTexFile"
//#define DateTime_Setting   L"DateTime"
//#define WindDirection_Setting L"WindDirection"
//#define WindStrong_Setting    L"WindStrong"
//#define Weather_Setting       L"Weather"
#define LogoFile_Setting   1
#define SkyTexFile_Setting 2
#define DateTime_Setting   3
#define WindDirection_Setting 4
#define WindStrong_Setting    5
#define Weather_Setting       6

#include "GlbObServerSet.h"
#include "glbref_ptr.h"

class GLB_DLLCLASS_EXPORT IGlbGlobeSettingListen:public CGlbReference
{
public:
	//virtual void SettingUpdate(const glbWChar* settingName){}
	virtual void SettingUpdate(glbInt32 settingName){}
};
class GLB_DLLCLASS_EXPORT CGlbGlobeSettings : public CGlbReference,
											  CGlbObserver
{
public:
	CGlbGlobeSettings(void);
	~CGlbGlobeSettings(void);
public:
	//logo文件
	CGlbWString const &GetLogoFile()const;
	void SetLogoFile(const glbWChar* logoFile);
	//天空纹理文件
	CGlbWString const &GetSkyTexFile()const;
	void SetSkyTexFile(const glbWChar* skytexFile);
	//时区
	CGlbWString const &GetTimeZone()const;
	void SetTimeZone(const glbWChar* timezone);
	//时间
	void SetDateTime(glbInt32 year,glbInt32 month,glbInt32 date,glbDouble houresUTC);
	void GetDateTime(glbInt32 &year,glbInt32 &month,glbInt32 &date,glbDouble &houresUTC)const;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//风向
	void SetWindDirection(glbDouble ws);
	glbDouble GetWindDirection()const;
	//风力
	void SetWindStrong(glbDouble ws);
	glbDouble GetWindStrong()const;
	//天气状况
	void SetWeather(glbDouble w);
	glbDouble GetWeather()const;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//太阳位置【地球球心 坐标系】
	const osg::Vec3d &GetSunPostion()const;
	//太阳到地球球心的距离
	glbDouble GetSunDistance()const;
	//月亮位置【地球球心 坐标系】
	const osg::Vec3d &GetMoonPostion()const;
	//月亮到地球球心距离
	glbDouble GetMoonDistance()const;
	//大气内圈半径
	glbDouble GetInnerAtmosphereRadius()const;
	//大气外圈半径
	glbDouble GetOuterAtmosphereRadius()const;
	//星空半径
	glbDouble GetStarsRadius()const;
	//星星的最小半径
	glbDouble GetMinStarMagnitude()const;
	//地球椭球体
	osg::EllipsoidModel *GetEarthEllipsoidModel()const;	
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void ObjectDeleted(void* listen);
	void RegisterListen(IGlbGlobeSettingListen* listen);
	void UnRegisterListen(IGlbGlobeSettingListen* listen);
private:
	//void notifyListens(const glbWChar* changedSetting);
	void notifyListens(glbInt32 changedSetting);
private:
	CGlbWString mpr_logoFile;     //logon 文件
	CGlbWString mpr_skytexFile;   //天空纹理文件
	CGlbWString mpr_timeZone;     //时区

	glbInt32 mpr_year,mpr_month,mpr_date;glbDouble mpr_houresUTC;          // UTC时间

	glbFloat                                mpr_earthRadius;               // 地球半径
	glbFloat                                mpr_innerRadius;               // 内圈大气半径
	glbFloat                                mpr_outerRadius;               // 外圈大气半径
	glbFloat                                mpr_sunDistance;               // 太阳距离
	glbFloat                mpr_starRadius, mpr_minStarMagnitude;          // 星空半径、星星最小半径

	osg::Vec3d                              mpr_sunPos;                    // 太阳位置
	osg::Vec3d                              mpr_moonPos;                   // 月亮位置

	glbDouble                               mpr_windstrong;
	glbDouble                               mpr_winddirection;
	glbDouble                               mpr_weather;

	std::vector<glbobserver_ptr<IGlbGlobeSettingListen>> mpr_listens;      //配置监听对象
	CRITICAL_SECTION                                     mpr_critical;     //监听对象添加删除临界区
};

