#pragma once
#include "GlbGlobeExport.h"
class GLB_DLLCLASS_EXPORT CGlbGlobeMemCtrl
{
public:
	/*
	*    对象 ReadData方法中，生成osgNode 成功后，
	*    先调用该方法。
	*    如果返回false，则放弃osgNode.
	*/
	glbBool  IncrementUsedMem(glbInt64 msize);
	void     DecrementUsedMem(glbInt64 msize);
	glbInt64 GetUsedMem();
	static CGlbGlobeMemCtrl* GetInstance();

	// 增加需编译数据量
	void	 IncrementTimeToCompileCost(glbDouble t);
	glbDouble GetTimeToCompileCost();
	void	 ClearTimeToCompileCost();
	void     ClearUsedMem();
protected:
	CGlbGlobeMemCtrl(void);
	~CGlbGlobeMemCtrl(void);
private:
	GlbCriticalSection      mpr_critical;
	glbInt64                mpr_hasUsedMem;
	static CGlbGlobeMemCtrl mpr_instance;
	glbDouble				mpr_timeToCompileCost;
};

