#pragma once

#include "GlbGlobeObjectFactory.h"

//< 点对象 创建器
class CGlbGlobePointProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobePointProvider();
	~CGlbGlobePointProvider();
	CGlbGlobeObject* Create();
};
//< 线对象 创建器
class CGlbGlobeLineProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeLineProvider();
	~CGlbGlobeLineProvider();
	CGlbGlobeObject* Create();
};
//< 面对象 创建器
class CGlbGlobePolygonProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobePolygonProvider();
	~CGlbGlobePolygonProvider();
	CGlbGlobeObject* Create();
};
//< Tin对象 创建器
class CGlbGlobeTinProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeTinProvider();
	~CGlbGlobeTinProvider();
	CGlbGlobeObject* Create();
};
//< 视线对象 创建器
class CGlbGlobeVisualLineAnalysisProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeVisualLineAnalysisProvider();
	~CGlbGlobeVisualLineAnalysisProvider();
	CGlbGlobeObject* Create();
};
//< 视域对象 创建器
class CGlbGlobeViewAnalysisProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeViewAnalysisProvider();
	~CGlbGlobeViewAnalysisProvider();
	CGlbGlobeObject* Create();
};
//< Complex对象 创建器
class CGlbGlobeComplexObjectProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeComplexObjectProvider();
	~CGlbGlobeComplexObjectProvider();
	CGlbGlobeObject* Create();
};
//< Drill对象 创建器
class CGlbGlobeDrillProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeDrillProvider();
	~CGlbGlobeDrillProvider();
	CGlbGlobeObject* Create();
};
//< DigHole对象 创建器
class CGlbGlobeDigHoleProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeDigHoleProvider();
	~CGlbGlobeDigHoleProvider();
	CGlbGlobeObject* Create();
};

//< Section对象 创建器
class CGlbGlobeSectionProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeSectionProvider();
	~CGlbGlobeSectionProvider();
	CGlbGlobeObject* Create();
};

//< Dynamiac对象 创建器
class CGlbGlobeDynamicProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeDynamicProvider();
	~CGlbGlobeDynamicProvider();
	CGlbGlobeObject* Create();
};

//< ModifyTerrain对象 创建器
class CGlbGlobeModifyTerrainProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeModifyTerrainProvider();
	~CGlbGlobeModifyTerrainProvider();
	CGlbGlobeObject* Create();
};

//< Video对象 创建器
class CGlbGlobeVideoProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeVideoProvider();
	~CGlbGlobeVideoProvider();
	CGlbGlobeObject* Create();
};

//< Network对象 创建器
class CGlbGlobeNetworkProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeNetworkProvider();
	~CGlbGlobeNetworkProvider();
	CGlbGlobeObject* Create();
};

//< CoordinateAxis对象 创建器
class CGlbGlobeCoordinateAxisProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeCoordinateAxisProvider();
	~CGlbGlobeCoordinateAxisProvider();
	CGlbGlobeObject* Create();
};

//< Grid对象从创建器
class CGlbGlobeGridProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeGridProvider();
	~CGlbGlobeGridProvider();
	CGlbGlobeObject* Create();
};

//< VirtualDrill对象 创建器
class CGlbGlobeVirtualDrillProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeVirtualDrillProvider();
	~CGlbGlobeVirtualDrillProvider();
	CGlbGlobeObject* Create();
};

//< VisualSection对象 创建器
class CGlbGlobeVirtualSectionProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeVirtualSectionProvider();
	~CGlbGlobeVirtualSectionProvider();
	CGlbGlobeObject* Create();
};

//< 对象创建注册器
class CGlbObjectRegisterCreator : public CGlbReference
{
public:
	CGlbObjectRegisterCreator(void);
	~CGlbObjectRegisterCreator(void);
};
