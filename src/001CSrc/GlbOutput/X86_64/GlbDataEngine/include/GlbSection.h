#pragma once
#include "GlbFeature.h"

/**
  * @brief 剖面对象。
  * @version 1.0
  * @date    2014-11-09
  * @author  吴自兴
  */
class GLBDATAENGINE_API CGlbSection : public CGlbFeature
{
public:
	CGlbSection(IGlbFeatureClass* cls,CGlbFields* fields);
	~CGlbSection(void);

public:
	/**
	  * @brief 查询剖面的控制点
	  *
	  * @param queryFilter 过滤器
	  * @return            游标,失败:NULL
	*/
	const IGlbObjectCursor* QueryCtrlPt(CGlbQueryFilter* queryFilter);
	/**
	  * @brief 查询剖面的节点
	  *
	  * @param queryFilter 过滤器
	  * @return            游标,失败:NULL
	*/
	const IGlbFeatureCursor* QueryNode(CGlbQueryFilter* queryFilter);
	/**
	  * @brief 查询剖面的弧段
	  *
	  * @param queryFilter 过滤器
	  * @return            游标,失败:NULL
	*/
	const IGlbFeatureCursor* QueryArc(CGlbQueryFilter* queryFilter);
	/**
	  * @brief 查询剖面的多边形
	  *
	  * @param queryFilter 过滤器
	  * @return            游标,失败:NULL
	*/
	const IGlbFeatureCursor* QueryPolygon(CGlbQueryFilter* queryFilter);
};