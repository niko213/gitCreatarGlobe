/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeDynamic.h
* @brief   动态对象公共头文件，文档定义CGlbGlobeDynamic类
* @version 1.0
* @author  敖建
* @date    2014-7-1 11:24
*********************************************************************/ 
#pragma once
//#include "GlbReference.h"
#include <osg/AnimationPath>

namespace GlbGlobe
{
	/**
	*枚举 动态对象类型
	*/
	enum GlbGlobeDynamicObjModeEnum
	{
		GLB_DYNAMICMODE_MODEL = 0,		//三维模型
		GLB_DYNAMICMODE_IMAGE = 1,		//图像注记
		GLB_DYNAMICMODE_TEXT = 2,		//文字注记
		GLB_DYNAMICMODE_VIRTUAL = 3		//虚拟（无）
	};
	/**
	*枚举 相机跟踪动态对象模式
	*/
	enum GlbGlobeDynamicTractModeEnum
	{ 
		GLB_DYNAMICTRACMODE_FIRST = 0,			//驾驶员视角
		GLB_DYNAMICTRACMODE_THIRD_TOP = 1,		//对象上方
		GLB_DYNAMICTRACMODE_THIRD_BOTTOM = 2,	//对象下方
		GLB_DYNAMICTRACMODE_THIRD_LEFT = 3,		//对象左边
		GLB_DYNAMICTRACMODE_THIRD_RIGHT = 4,	//对象右边
		GLB_DYNAMICTRACMODE_THIRD_FRONT = 5,	//对象前面
		GLB_DYNAMICTRACMODE_THIRD_BACK = 6,		//对象后面
		GLB_DYNAMICTRACMODE_THIRD_BACKTOP = 7,	//对象后上方
		GLB_DYNAMICTRACMODE_NONE = 8			//相机不跟踪对象
	};
	/**
	*枚举 动态对象运动模式（会影响到动态对象运动时yaw，pitch，roll的计算算法）
	*/
	enum GlbGlobeDynamicPlayModeEnum
	{
		GLB_DYNAMICPLAYMODE_PERSON = 0,		//人
		GLB_DYNAMICPLAYMODE_CAR = 1,		//地面车辆
		GLB_DYNAMICPLAYMODE_AIRPLANE = 2,	//飞机
		GLB_DYNAMICPLAYMODE_HELICOPTER = 3	//直升飞机
	};
	/**
	* @brief CGlbGlobeDynamic 动态对象接口	 
	* @version 1.0
	* @author  敖建
	* @date    2014-7-1 11:24
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeDynamic //: virtual public CGlbReference
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeDynamic(void);
		/**
		* @brief 析构函数
		*/
		virtual ~CGlbGlobeDynamic(void);
		/**
		* @brief 添加控制点
		* @param [in] 控制点位置xOrlon,yOrlat,zOrAlt（球模式为经纬高，经度纬度单位为°，海拔高度单位为米；平面模式单位为米）
		*		  [in] 控制点姿态yaw，pitch,roll(单位为°)
		*		  [in] 控制点时间 seconds（单位为秒）
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool AddPosition(glbDouble xOrlon,glbDouble yOrlat,glbDouble zOrAlt,glbDouble yaw,glbDouble pitch,glbDouble roll,glbDouble seconds) = 0;
		/**
		* @brief 插入控制点
		* @param [in] 控制点编号 nId
		*		  [in] 控制点位置xOrlon,yOrlat,zOrAlt（球模式为经纬高，经度纬度单位为°，海拔高度单位为米；平面模式单位为米）
		*		  [in] 控制点姿态yaw，pitch,roll(单位为°)
		*		  [in] 控制点时间 seconds（单位为秒）
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool InsertPosition(glbInt32 nId,glbDouble xOrlon,glbDouble yOrlat,glbDouble zOrAlt,glbDouble yaw,glbDouble pitch,glbDouble roll,glbDouble seconds) = 0;
		/**
		* @brief 替换控制点
		* @param [in] 控制点编号 nId
		*		  [in] 控制点位置xOrlon,yOrlat,zOrAlt（球模式为经纬高，经度纬度单位为°，海拔高度单位为米；平面模式单位为米）
		*		  [in] 控制点姿态yaw，pitch,roll(单位为°)
		*		  [in] 控制点时间 seconds（单位为秒）
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool ReplacePosition(glbInt32 nId,glbDouble xOrlon,glbDouble yOrlat,glbDouble zOrAlt,glbDouble yaw,glbDouble pitch,glbDouble roll,glbDouble seconds) = 0;
		/**
		* @brief 移除控制点
		* @param [in] 控制点编号 nId
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool RemovePosition(glbInt32 nId) = 0;
		/**
		* @brief 获取控制点个数
		* @return 控制点个数
		*/
		virtual glbInt32 GetCount() = 0;
		/**
		* @brief 获取控制点信息
		* @param [in] 控制点编号 nId
		*		  [out] 控制点位置*xOrlon,*yOrlat,*zOrAlt（球模式为经纬高，经度纬度单位为°，海拔高度单位为米；平面模式单位为米）
		*		  [out] 控制点姿态*yaw,*pitch,*roll(单位为°)
		*		  [out] 控制点时间 *seconds（单位为秒）
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool GetPosition(glbInt32 nId,glbDouble* xOrlon,glbDouble* yOrlat,glbDouble* zOrAlt,glbDouble* yaw,glbDouble* pitch,glbDouble* roll,glbDouble* seconds) = 0;
		/**
		* @brief 获取当前时刻控制点信息
		* @param [out] 控制点*cp
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool GetControlPoint(osg::AnimationPath::ControlPoint *cp) = 0;
		///**
		// * @brief 获取当前时刻动态对象位置姿态矩阵
		//**/
		//virtual osg::Matrixd GetMatrix() = 0;
		/**
		* @brief 设置动态对象类型
		* @param [in] GlbGlobeDymicObjModeEnum 动态对象类型枚举类型
		*/
		virtual void SetDynamicObjMode(GlbGlobeDynamicObjModeEnum mode);
		/**
		* @brief 获取动态对象类型
		* @return  GlbGlobeDymicObjModeEnum 动态对象类型枚举类型
		*/
		virtual GlbGlobeDynamicObjModeEnum GetDynamicObjMode();
		/**
		* @brief 设置动态对象跟踪模式
		* @param [in] GlbGlobeDymicTractModeEnum 动态对象跟踪模式枚举类型
		*/
		virtual void SetTraceMode(GlbGlobeDynamicTractModeEnum mode);
		/**
		* @brief 获取动态对象跟踪模式
		* @return  GlbGlobeDymicTractModeEnum 动态对象跟踪模式枚举类型
		*/
		virtual GlbGlobeDynamicTractModeEnum GetTraceMode();
		/**
		* @brief 设置动态对象运动模式
		* @param [in] GlbGlobeDymicPlayModeEnum 动态对象运动模式枚举类型
		*/
		virtual void SetPlayMode(GlbGlobeDynamicPlayModeEnum mode);
		/**
		* @brief 获取动态对象运动模式
		* @return  GlbGlobeDymicPlayModeEnum 动态对象运动模式枚举类型
		*/
		virtual GlbGlobeDynamicPlayModeEnum GetPlayMode();
		/**
		* @brief 设置动态对象第三人称跟踪模式下对象——相机距离
		* @param [in] distan 动态对象第三人称跟踪模式下对象——相机距离
		*/
		virtual void SetThirdModeDistance(glbDouble distan);
		/**
		* @brief 获取动态对象第三人称跟踪模式下对象——相机距离
		* @return  动态对象第三人称跟踪模式下对象——相机距离
		*/
		virtual glbDouble GetThirdModeDistance();
		/**
		* @brief 设置动态对象第三人称跟踪模式下观察相机Picth角
		* @param [in] pitch 动态对象第三人称跟踪模式下观察相机Picth角,一般输入为负值，单位为°
		*/
		virtual void SetThirdModePitch(glbDouble pitch);
		/**
		* @brief 获取动态对象第三人称跟踪模式下观察相机Picth角
		* @return  动态对象第三人称跟踪模式下观察相机Picth角
		*/
		virtual glbDouble GetThirdModePitch();
		/**
		* @brief 设置动态对象是否重复
		* @param [in] isrepeat 动态对象是否重复
		*/
		virtual void SetRepeat(glbBool isrepeat);
		/**
		* @brief 获取动态对象是否重复
		* @return  动态对象是否重复
		*/
		virtual glbBool GetRepeat();
	protected:
		GlbGlobeDynamicObjModeEnum		mpr_dymicmode;	//动态对象类型
		GlbGlobeDynamicTractModeEnum	mpr_tractmode;	//动态对象跟踪模式枚举类型
		GlbGlobeDynamicPlayModeEnum		mpr_playmode;	//动态对象运动模式
		glbDouble						mpr_ditan;		//第三人称跟踪模式下对象——相机距离
		glbDouble						mpr_pitch;		//第三人称跟踪模式下观察相机Picth角
		glbBool							mpr_isrepeat;	//对象是否重复
	};
}	
