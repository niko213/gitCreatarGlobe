/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbCompareValue.h
* @brief   数值比较类，文档定义CGlbCompareValue类
* @version 1.0
* @author  敖建
* @date    2014-8-14 13:30
*********************************************************************/
#pragma once
#include "GlbGlobeObjectExport.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "GlbFeature.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbCompareValue类				 
	* @version 1.0
	* @author  敖建
	* @date    2014-8-14 13:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbCompareValue
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbCompareValue(void);
		/**
		* @brief 析构函数
		*/
		~CGlbCompareValue(void);
	public:
		/**
		* @brief 判断两个glbDouble的值是否相等
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareDoubleEqual(glbDouble leftValue,glbDouble rightValue);
		/**
		* @brief 判断两个GlbRenderDouble的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueDoubleEqual(GlbRenderDouble *leftValue,GlbRenderDouble *rightValue,CGlbFeature *feature);
		/**
		* @brief 判断两个GlbRenderInt32的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueIntEqual(GlbRenderInt32 *leftValue,GlbRenderInt32 *rightValue,CGlbFeature *feature);
		/**
		* @brief 判断两个GlbRenderBool的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueBoolEqual(GlbRenderBool *leftValue,GlbRenderBool *rightValue,CGlbFeature *feature);
		/**
		* @brief 判断两个GlbRenderColor的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueColorEqual(GlbRenderColor *leftValue,GlbRenderColor *rightValue,CGlbFeature *feature);
		/**
		* @brief 判断两个GlbRenderString的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueStringEqual(GlbRenderString *leftValue,GlbRenderString *rightValue,CGlbFeature *feature);
		/**
		* @brief 判断两个GlbRenderTexRepeatMode的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueTexRepeatEnumEqual(GlbRenderTexRepeatMode *leftValue,GlbRenderTexRepeatMode *rightValue,CGlbFeature *feature);
		/**
		* @brief 判断两个GlbRenderLinePattern的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueLinePatternEnumEqual(GlbRenderLinePattern *leftValue,GlbRenderLinePattern *rightValue,CGlbFeature *feature);
		/**
		* @brief 判断两个GlbRenderMultilineAlign的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueMultilineAlignEnumEqual(GlbRenderMultilineAlign *leftValue,GlbRenderMultilineAlign *rightValue,CGlbFeature *feature);
		/**
		* @brief 判断两个GlbRenderLabelRelation的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueLabelRelationEnumEqual(GlbRenderLabelRelation *leftValue,GlbRenderLabelRelation *rightValue,CGlbFeature *feature);
		/**
		* @brief 判断两个GlbRenderLabelAlign的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueLabelAlignEnumEqual(GlbRenderLabelAlign *leftValue,GlbRenderLabelAlign *rightValue,CGlbFeature *feature);
		/**
		* @brief 判断两个GlbRenderImageAlign的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueImageAlignEnumEqual(GlbRenderImageAlign *leftValue,GlbRenderImageAlign *rightValue,CGlbFeature *feature);
		/**
		* @brief 判断两个GlbRenderBillboard的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueBillboardEnumEqual(GlbRenderBillboard *leftValue,GlbRenderBillboard *rightValue,CGlbFeature *feature);
		/**
		* @brief 判断两个GlbRenderArrowPattern的值是否有修改
		* @return  左值右值相同返回true，否则返回false
		*/
		static glbBool CompareValueArrowPatternEnumEqual(GlbRenderArrowPattern *leftValue,GlbRenderArrowPattern *rightValue,CGlbFeature *feature);

		static glbBool CompareValueDynamicLabelFadeEnumEqual(GlbRenderDynamicLabelFadeMode *leftValue,GlbRenderDynamicLabelFadeMode *rightValue,CGlbFeature *feature);

		static glbBool CompareValueDynamicLabelFadeSpeedEnumEqual(GlbRenderDynamicLabelFadeSpeed *leftValue,GlbRenderDynamicLabelFadeSpeed *rightValue,CGlbFeature *feature);
	};
}
