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
#include "GlbGlobeSymbolExport.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "GlbFeature.h"

namespace GlbGlobe
{
	/*
	* @brief CGlbCompareValue类				 
	* @version 1.0
	* @author  敖建
	* @date    2014-8-14 13:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeCommon
	{
	public:
		CGlbGlobeCommon(void);
		~CGlbGlobeCommon(void);
	public:
		/**
		* @brief 左值右值相同返回true，否则返回false
		**/
		static glbBool CompareDoubleEqual(glbDouble leftValue,glbDouble rightValue);
		static glbBool CompareValueDoubleEqual(GlbRenderDouble *leftValue,GlbRenderDouble *rightValue,CGlbFeature *feature);
		static glbBool CompareValueIntEqual(GlbRenderInt32 *leftValue,GlbRenderInt32 *rightValue,CGlbFeature *feature);
		static glbBool CompareValueBoolEqual(GlbRenderBool *leftValue,GlbRenderBool *rightValue,CGlbFeature *feature);
		static glbBool CompareValueColorEqual(GlbRenderColor *leftValue,GlbRenderColor *rightValue,CGlbFeature *feature);
		static glbBool CompareValueStringEqual(GlbRenderString *leftValue,GlbRenderString *rightValue,CGlbFeature *feature);
		static glbBool CompareValueTexRepeatEnumEqual(GlbRenderTexRepeatMode *leftValue,GlbRenderTexRepeatMode *rightValue,CGlbFeature *feature);
		static glbBool CompareValueLinePatternEnumEqual(GlbRenderLinePattern *leftValue,GlbRenderLinePattern *rightValue,CGlbFeature *feature);
		static glbBool CompareValueMultilineAlignEnumEqual(GlbRenderMultilineAlign *leftValue,GlbRenderMultilineAlign *rightValue,CGlbFeature *feature);
		static glbBool CompareValueLabelRelationEnumEqual(GlbRenderLabelRelation *leftValue,GlbRenderLabelRelation *rightValue,CGlbFeature *feature);
		static glbBool CompareValueLabelAlignEnumEqual(GlbRenderLabelAlign *leftValue,GlbRenderLabelAlign *rightValue,CGlbFeature *feature);
		static glbBool CompareValueImageAlignEnumEqual(GlbRenderImageAlign *leftValue,GlbRenderImageAlign *rightValue,CGlbFeature *feature);
		static glbBool CompareValueBillboardEnumEqual(GlbRenderBillboard *leftValue,GlbRenderBillboard *rightValue,CGlbFeature *feature);

		static glbBool CompareValueArrowPatternEnumEqual(GlbRenderArrowPattern *leftValue,GlbRenderArrowPattern *rightValue,CGlbFeature *feature);
	};
}