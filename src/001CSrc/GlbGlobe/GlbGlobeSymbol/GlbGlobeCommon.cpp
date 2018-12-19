#include "StdAfx.h"
#include "GlbGlobeCommon.h"
using namespace GlbGlobe;

CGlbGlobeCommon::CGlbGlobeCommon(void)
{
}

CGlbGlobeCommon::~CGlbGlobeCommon(void)
{
}

glbBool CGlbGlobeCommon::CompareDoubleEqual(glbDouble leftValue,glbDouble rightValue)
{
	if (fabs(leftValue - rightValue) > DBL_EPSILON /*0.0000001*/)
		return false;
	return true;
}

glbBool CGlbGlobeCommon::CompareValueDoubleEqual(GlbRenderDouble *leftValue,GlbRenderDouble *rightValue,CGlbFeature *feature)
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

glbBool CGlbGlobeCommon::CompareValueColorEqual(GlbRenderColor *leftValue,GlbRenderColor *rightValue,CGlbFeature *feature)
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

glbBool CGlbGlobeCommon::CompareValueIntEqual(GlbRenderInt32 *leftValue,GlbRenderInt32 *rightValue,CGlbFeature *feature)
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

glbBool CGlbGlobeCommon::CompareValueBoolEqual(GlbRenderBool *leftValue,GlbRenderBool *rightValue,CGlbFeature *feature)
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

glbBool CGlbGlobeCommon::CompareValueStringEqual(GlbRenderString *leftValue,GlbRenderString *rightValue,CGlbFeature *feature)
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

glbBool CGlbGlobeCommon::CompareValueTexRepeatEnumEqual(GlbRenderTexRepeatMode *leftValue,GlbRenderTexRepeatMode *rightValue,CGlbFeature *feature)
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

glbBool CGlbGlobeCommon::CompareValueLinePatternEnumEqual(GlbRenderLinePattern *leftValue,GlbRenderLinePattern *rightValue,CGlbFeature *feature)
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

glbBool CGlbGlobeCommon::CompareValueMultilineAlignEnumEqual(GlbRenderMultilineAlign *leftValue,GlbRenderMultilineAlign *rightValue,CGlbFeature *feature)
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

glbBool CGlbGlobeCommon::CompareValueLabelRelationEnumEqual(GlbRenderLabelRelation *leftValue,GlbRenderLabelRelation *rightValue,CGlbFeature *feature)
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

glbBool CGlbGlobeCommon::CompareValueLabelAlignEnumEqual(GlbRenderLabelAlign *leftValue,GlbRenderLabelAlign *rightValue,CGlbFeature *feature)
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

glbBool CGlbGlobeCommon::CompareValueImageAlignEnumEqual(GlbRenderImageAlign *leftValue,GlbRenderImageAlign *rightValue,CGlbFeature *feature)
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

glbBool CGlbGlobeCommon::CompareValueBillboardEnumEqual(GlbRenderBillboard *leftValue,GlbRenderBillboard *rightValue,CGlbFeature *feature)
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

glbBool CGlbGlobeCommon::CompareValueArrowPatternEnumEqual(GlbRenderArrowPattern *leftValue,GlbRenderArrowPattern *rightValue,CGlbFeature *feature)
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