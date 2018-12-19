/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeREObject.h
* @brief   可编辑绘制对象头文件，文档定义CGlbGlobeREObject类
* @version 1.0
* @author  ChenPeng
* @date    2014-5-6 15:00
*********************************************************************/
#pragma once
#include "GlbGlobeRObject.h"

namespace GlbGlobe
{
	/**
	* @brief 可编辑绘制对象类				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-6 10:50
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeREObject : public CGlbGlobeRObject
	{
	public:
		/**
		* @brief 设置对象海拔模式
		* @param[in]  mode 海拔模式枚举
		* @param[out] isNeedDirectDraw 是否需要调用DirectDraw才能立即看到修改效果：true需要,false不需要.
		* @return 设置成功返回true，设置失败返回false
		*/
		virtual glbBool SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw);
		/**
		* @brief 设置对象最大可见距离
		* @param[in]  distance 对象最大可见距离
		* @return 设置成功返回true，设置失败返回false
		*/
		virtual glbBool SetMaxVisibleDistance(glbDouble distance);
		/**
		* @brief 设置对象最小可见距离
		* @param[in]  distance 对象最小可见距离
		* @return 设置成功返回true，设置失败返回false
		*/
		virtual glbBool SetMinVisibleDistance(glbDouble distance);
		/**
		* @brief 设置对象渲染优先级
		* @param[in]  order 对象渲染优先级
		*/
		virtual void SetRenderOrder(glbInt32 order);
		/**
		* @brief 设置对象装载优先级
		* @param[in]  order 对象装载优先级
		*/
		virtual void SetLoadOrder(glbInt32 order);
		/**
		* @brief 设置对象提示信息
		* @param[in]  tooltip 对象提示信息
		* @return 设置成功返回true，设置失败返回false
		* @note 当鼠标放到对象上时会显示出来
		*/
		virtual glbBool SetToolTip(/*CGlbWString*/ glbWChar* tooltip);
		/**
		* @brief 设置对象信息
		* @param[in]  msg 对象信息
		* @return 设置成功返回true，设置失败返回false
		* @note 当点击对象时会显示出来
		*/
		virtual glbBool SetRMessage(/*CGlbWString*/ glbWChar* msg);
	};
}