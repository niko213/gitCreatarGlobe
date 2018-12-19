/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    IGlbGlobeViewEventHandle.h
* @brief   CGlbGlobeView的事件监听抽象类接口定义
*
* 这个档案定义IGlbGlobeViewEventHandle这个class,
*
* @version 1.0
* @author  敖建
* @date    2014-5-21 10:40
*********************************************************************/
#pragma once

#include  "GlbGlobeExport.h"
#include "GlbGlobeTypes.h"
namespace GlbGlobe
{
	/**
	*枚举 鼠标左中右键枚举类型
	*/
	enum GlbGlobeMouseButtonEnum
	{
		GLB_LEFT_MOUSEBUTTON = 0,	//鼠标左键
		GLB_MIDDLE_MOUSEBUTTON = 1,	//鼠标中键
		GLB_RIGHT_MOUSEBUTTON = 2	//鼠标右键
	};
	class  GLB_DLLCLASS_EXPORT IGlbGlobeViewEventHandle : public CGlbReference
	{
	public:
		/**
		* @brief 帧绘制结束事件的响应方法,视图在每帧绘制结束后调用
		*/
		virtual void OnFrame()=0;
		/**
		* @brief 鼠标按下事件
		* @param [in] button ：GlbGlobeMouseButtonEnum类型，按下的鼠标
		*		  [in] nflags ：鼠标事件发生时，同时按下的其他控制键，取值：
		*						MK_CONTROL
		*						MK_LBUTTON
		*						MK_MBUTTON
		*						MK_RBUTTON
		*						MK_SHIFT
		*		  [in] wx,wy : 鼠标在窗口的x轴,y轴位置
		* @return false，表示没有处理消息，系统需要继续处理该消息
		*		-	true, 表示处理了消息，系统不能继续处理该消息
		*/
		virtual glbBool OnButtonDown(GlbGlobeMouseButtonEnum button,glbInt32 nflags, glbInt32 wx, glbInt32 wy)=0;
		/**
		* @brief 鼠标弹起事件
		* @param [in] button ：GlbGlobeMouseButtonEnum类型，按下的鼠标
		*		  [in] nflags ：鼠标事件发生时，同时按下的其他控制键，取值：
		*						MK_CONTROL
		*						MK_LBUTTON
		*						MK_MBUTTON
		*						MK_RBUTTON
		*						MK_SHIFT
		*		  [in] wx,wy : 鼠标在窗口的x轴,y轴位置
		* @return false，表示没有处理消息，系统需要继续处理该消息
		*		-	true, 表示处理了消息，系统不能继续处理该消息
		*/
		virtual glbBool OnButtonUp(GlbGlobeMouseButtonEnum button,glbInt32 nflags, glbInt32 wx, glbInt32 wy)=0;
		/**
		* @brief 鼠标滚轮事件的响应方法，视图在发生鼠标滚轮消息时调用。
		* @param [in] nflags ：鼠标事件发生时，同时按下的其他控制键，取值：
		*						MK_CONTROL
		*						MK_LBUTTON
		*						MK_MBUTTON
		*						MK_RBUTTON
		*						MK_SHIFT
		*		  [in] delta : 滚动刻度，>0 向前滚动，<0 向后滚动
		*		  [in] wx,wy : 鼠标在窗口的x轴,y轴位置
		* @return false，表示没有处理消息，系统需要继续处理该消息
		*		-	true, 表示处理了消息，系统不能继续处理该消息
		*/
		virtual glbBool OnMouseWheel(glbInt32 nflags, glbInt32 delta, glbInt32 wx, glbInt32 wy)=0;
		/**
		* @brief 鼠标双击事件
		* @param [in] button ：GlbGlobeMouseButtonEnum类型，按下的鼠标
		*		  [in] nflags ：鼠标事件发生时，同时按下的其他控制键，取值：
		*						MK_CONTROL
		*						MK_LBUTTON
		*						MK_MBUTTON
		*						MK_RBUTTON
		*						MK_SHIFT
		*		  [in] wx,wy : 鼠标在窗口的x轴,y轴位置
		* @return false，表示没有处理消息，系统需要继续处理该消息
		*		-	true, 表示处理了消息，系统不能继续处理该消息
		*/
		virtual glbBool OnButtonDblClk(GlbGlobeMouseButtonEnum button,glbInt32 nflags, glbInt32 wx, glbInt32 wy)=0;
		/**
		* @brief 鼠标移动事件的响应方法，视图在发生鼠标移动消息时调用
		* @param [in] nflags ：鼠标事件发生时，同时按下的其他控制键，取值：
		*						MK_CONTROL
		*						MK_LBUTTON
		*						MK_MBUTTON
		*						MK_RBUTTON
		*						MK_SHIFT
		*		  [in] wx,wy : 鼠标在窗口的x轴,y轴位置
		* @return false，表示没有处理消息，系统需要继续处理该消息
		*		-	true, 表示处理了消息，系统不能继续处理该消息
		*/
		virtual glbBool OnMouseMove(glbInt32 nflags, glbInt32 wx, glbInt32 wy)=0;
		/**
		* @brief 按下键盘事件的响应方法，视图在发生键盘按下消息时调用
		* @param [in] keycode ：即WM_KEYDOWN/WM_SYSKEYDOWN的WParam
		*		  [in] nflags : 即WM_KEYDOWN/WM_SYSKEYDOWN的LParam
		* @return false，表示没有处理消息，系统需要继续处理该消息
		*		-	true, 表示处理了消息，系统不能继续处理该消息
		*/
		virtual glbBool OnKeyDown(glbInt32 keycode, glbInt32 nflags)=0;
		/**
		* @brief 键盘弹起事件的响应方法，视图在发生键盘弹起消息时调用
		* @param [in] keycode ：即WM_KEYDOWN/WM_SYSKEYDOWN的WParam
		*		  [in] nflags : 即WM_KEYDOWN/WM_SYSKEYDOWN的LParam
		* @return false，表示没有处理消息，系统需要继续处理该消息
		*		-	true, 表示处理了消息，系统不能继续处理该消息
		*/
		virtual glbBool OnKeyUp(glbInt32 keycode, glbInt32 nflags)=0;
	};
}