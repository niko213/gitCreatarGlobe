#pragma once
#include "GlbGlobeExport.h"
#include "GlbCommTypes.h"
#include "glbref_ptr.h"
#include "GlbReference.h"

class GLB_DLLCLASS_EXPORT CGlbGlobalConfig : public CGlbReference
{
public:
	static  CGlbGlobalConfig* const GetInstance();
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//调度 - 内存
	glbBool IsMemCtrl();
	void SetMemCtrl(glbBool isMemCtrl);
	glbDouble GetUnusedMem();
	void SetUnusedMem(glbDouble unusedMem);
	//调度 - 显存
	glbBool IsDMemCtrl();
	void SetDMemCtrl(glbBool isDMemCtrl);
	glbDouble GetTotalDMem();
	void     SetTotalDMem(glbDouble totalDMem);
protected:
	CGlbGlobalConfig(void);
	~CGlbGlobalConfig(void);
private:
	static glbref_ptr<CGlbGlobalConfig> mpr_intance;
};

