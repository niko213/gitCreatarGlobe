#pragma once
#include "GlbFeature.h"

/**
  * @brief 钻孔对象。
  * @version 1.0
  * @date    2014-11-09
  * @author  吴自兴
  */
class GLBDATAENGINE_API CGlbDrill :	public CGlbFeature
{
public:
	CGlbDrill(IGlbFeatureClass* cls,CGlbFields* fields);
	~CGlbDrill(void);

public:
	/**
	  * @brief             查询地层
	  *
	  * @param queryFilter 查询条件。
	  * @return            游标,失败NULL
	*/
	const IGlbFeatureCursor* QueryStratum(CGlbQueryFilter* queryFilter);
	/**
	  * @brief             查询侧斜
	  *
	  * @param queryFilter 查询条件。
	  * @return            游标,失败NULL
	*/
	const IGlbObjectCursor* QuerySkew(CGlbQueryFilter* queryFilter);
	/**
	  * @brief             查询样品
	  *
	  * @param queryFilter 查询条件。
	  * @return            游标,失败NULL
	*/
	const IGlbObjectCursor* QuerySample(CGlbQueryFilter* queryFilter, const glbWChar* spDatasetName);
};

