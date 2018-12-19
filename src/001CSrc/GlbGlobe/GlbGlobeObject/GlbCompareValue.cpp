#include "StdAfx.h"
#include "GlbCompareValue.h"
using namespace GlbGlobe;

CGlbCompareValue::CGlbCompareValue(void)
{
}


CGlbCompareValue::~CGlbCompareValue(void)
{
}

glbBool CGlbCompareValue::CompareDoubleEqual(glbDouble leftValue,glbDouble rightValue)
{
	if (fabs(leftValue - rightValue) > 0.0000001)
		return false;
	return true;
}

glbBool CGlbCompareValue::CompareValueDoubleEqual(GlbRenderDouble *leftValue,GlbRenderDouble *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return CompareDoubleEqual(leftValue->GetValue(feature),rightValue->GetValue(feature));
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool CGlbCompareValue::CompareValueColorEqual(GlbRenderColor *leftValue,GlbRenderColor *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
	{
		if( leftValue->bUseField != rightValue->bUseField )
			return false;
		if(leftValue->bUseField && rightValue->bUseField && leftValue->field != rightValue->field)
			return false;
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	}
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool CGlbCompareValue::CompareValueIntEqual(GlbRenderInt32 *leftValue,GlbRenderInt32 *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool CGlbCompareValue::CompareValueBoolEqual(GlbRenderBool *leftValue,GlbRenderBool *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool CGlbCompareValue::CompareValueStringEqual(GlbRenderString *leftValue,GlbRenderString *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return wcscmp(leftValue->GetValue(feature),rightValue->GetValue(feature)) == 0;
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool CGlbCompareValue::CompareValueTexRepeatEnumEqual(GlbRenderTexRepeatMode *leftValue,GlbRenderTexRepeatMode *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool CGlbCompareValue::CompareValueLinePatternEnumEqual(GlbRenderLinePattern *leftValue,GlbRenderLinePattern *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool CGlbCompareValue::CompareValueMultilineAlignEnumEqual(GlbRenderMultilineAlign *leftValue,GlbRenderMultilineAlign *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool CGlbCompareValue::CompareValueLabelRelationEnumEqual(GlbRenderLabelRelation *leftValue,GlbRenderLabelRelation *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool CGlbCompareValue::CompareValueLabelAlignEnumEqual(GlbRenderLabelAlign *leftValue,GlbRenderLabelAlign *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool CGlbCompareValue::CompareValueImageAlignEnumEqual(GlbRenderImageAlign *leftValue,GlbRenderImageAlign *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool CGlbCompareValue::CompareValueBillboardEnumEqual(GlbRenderBillboard *leftValue,GlbRenderBillboard *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool CGlbCompareValue::CompareValueArrowPatternEnumEqual(GlbRenderArrowPattern *leftValue,GlbRenderArrowPattern *rightValue,CGlbFeature *feature)
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool GlbGlobe::CGlbCompareValue::CompareValueDynamicLabelFadeEnumEqual( GlbRenderDynamicLabelFadeMode *leftValue,GlbRenderDynamicLabelFadeMode *rightValue,CGlbFeature *feature )
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}

glbBool GlbGlobe::CGlbCompareValue::CompareValueDynamicLabelFadeSpeedEnumEqual( GlbRenderDynamicLabelFadeSpeed *leftValue,GlbRenderDynamicLabelFadeSpeed *rightValue,CGlbFeature *feature )
{
	if(leftValue == NULL && rightValue == NULL)
		return true;
	if(leftValue != NULL && rightValue != NULL)
		return leftValue->GetValue(feature) == rightValue->GetValue(feature);
	if(leftValue != NULL && rightValue == NULL)
		return false;
	if(leftValue == NULL && rightValue != NULL)
		return false;
}