#include "StdAfx.h"
#include "GlbGlobePoint.h"
#include "GlbGlobeLine.h"
#include "GlbGlobePolygon.h"
#include "GlbObjectRegisterCreator.h"
#include "GlbGlobeLine.h"
#include "GlbGlobePolygon.h"
#include "GlbGlobeTinObject.h"
#include "GlbGlobeVisualLineAnalysis.h"
#include "GlbGlobeViewAnalysis.h"
#include "GlbGlobeCompositeObject.h"
#include "GlbGlobeDrill.h"
#include "GlbGlobeSection.h"
#include "GlbGlobeDigHole.h"
#include "GlbGlobeDynamicObject.h"
#include "GlbGlobeTerrainModify.h"
#include "GlbGlobeVideo.h"
#include "GlbGlobeNetworkObject.h"
#include "GlbGlobeCoordinateAxisObject.h"
#include "GlbGlobeGridObject.h"
#include "GlbGlobeVirtualDrillObject.h"
#include "GlbGlobeVirtualSectionObject.h"

CGlbObjectRegisterCreator::CGlbObjectRegisterCreator(void)
{
	CGlbGlobeObjectFactory *factory = CGlbGlobeObjectFactory::GetInstance();		

	factory->Register(new CGlbGlobePointProvider(),L"GlbGlobePoint");
	factory->Register(new CGlbGlobeLineProvider(),L"GlbGlobeLine");
	factory->Register(new CGlbGlobePolygonProvider(),L"GlbGlobePolygon");
	factory->Register(new CGlbGlobeTinProvider(),L"GlbGlobeTin");
	factory->Register(new CGlbGlobeVisualLineAnalysisProvider(),L"GlbGlobeVisualLineAnalysis");
	factory->Register(new CGlbGlobeViewAnalysisProvider(),L"GlbGlobeViewAnalysis");
	factory->Register(new CGlbGlobeComplexObjectProvider(),L"GlbGlobeComplexObject");
	factory->Register(new CGlbGlobeDrillProvider(),L"GlbGlobeDrill");
	factory->Register(new CGlbGlobeSectionProvider(),L"GlbGlobeSection");
	factory->Register(new CGlbGlobeDigHoleProvider(),L"GlbGlobeDigHole");
	factory->Register(new CGlbGlobeDynamicProvider(),L"GlbGlobeDynamic");
	factory->Register(new CGlbGlobeModifyTerrainProvider(),L"GlbGlobeModifyTerrain");
	factory->Register(new CGlbGlobeVideoProvider(),L"GlbGlobeVideo");
	factory->Register(new CGlbGlobeNetworkProvider(),L"GlbGlobeNetwork");
	factory->Register(new CGlbGlobeCoordinateAxisProvider(),L"GlbGlobeCoordinateAxis");
	factory->Register(new CGlbGlobeGridProvider(),L"GlbGlobeGrid");
	factory->Register(new CGlbGlobeVirtualDrillProvider(),L"GlbGlobeVisualDrill");
	factory->Register(new CGlbGlobeVirtualSectionProvider(),L"GlbGlobeVisualSection");
}


CGlbObjectRegisterCreator::~CGlbObjectRegisterCreator(void)
{
	CGlbGlobeObjectFactory *factory = CGlbGlobeObjectFactory::GetInstance();		

	factory->UnRegister(L"GlbGlobeDrill");
	factory->UnRegister(L"GlbGlobeComplexObject");
	factory->UnRegister(L"GlbGlobeViewAnalysis");
	factory->UnRegister(L"GlbGlobeVisualLineAnalysis");
	factory->UnRegister(L"GlbGlobeTin");
	factory->UnRegister(L"GlbGlobePolygon");
	factory->UnRegister(L"GlbGlobeLine");
	factory->UnRegister(L"GlbGlobePoint");
	factory->UnRegister(L"GlbGlobeSection");
	factory->UnRegister(L"GlbGlobeDigHole");
	factory->UnRegister(L"GlbGlobeDynamic");
	factory->UnRegister(L"GlbGlobeModifyTerrain");
	factory->UnRegister(L"GlbGlobeVideo");
	factory->UnRegister(L"GlbGlobeNetwork");
	factory->UnRegister(L"GlbGlobeCoordinateAxis");
	factory->UnRegister(L"GlbGlobeGrid");
	factory->UnRegister(L"GlbGlobeVisualDrill");
	factory->UnRegister(L"GlbGlobeVisualSection");
}

CGlbGlobePointProvider::CGlbGlobePointProvider()
{

}
CGlbGlobePointProvider::~CGlbGlobePointProvider()
{

}
CGlbGlobeObject* CGlbGlobePointProvider::Create()
{
	return new CGlbGlobePoint();
}

CGlbGlobeLineProvider::CGlbGlobeLineProvider()
{

}
CGlbGlobeLineProvider::~CGlbGlobeLineProvider()
{

}
CGlbGlobeObject* CGlbGlobeLineProvider::Create()
{
	return new CGlbGlobeLine();
}

CGlbGlobePolygonProvider::CGlbGlobePolygonProvider()
{

}
CGlbGlobePolygonProvider::~CGlbGlobePolygonProvider()
{

}
CGlbGlobeObject* CGlbGlobePolygonProvider::Create()
{
	return new CGlbGlobePolygon();
}

CGlbGlobeTinProvider::CGlbGlobeTinProvider()
{

}
CGlbGlobeTinProvider::~CGlbGlobeTinProvider()
{

}
CGlbGlobeObject* CGlbGlobeTinProvider::Create()
{
	return new CGlbGlobeTinObject();
	//return new CGlbGlobePolygon();
}

CGlbGlobeVisualLineAnalysisProvider::CGlbGlobeVisualLineAnalysisProvider()
{

}
CGlbGlobeVisualLineAnalysisProvider::~CGlbGlobeVisualLineAnalysisProvider()
{

}
CGlbGlobeObject* CGlbGlobeVisualLineAnalysisProvider::Create()
{
	return new CGlbGlobeVisualLineAnalysis();
}

CGlbGlobeViewAnalysisProvider::CGlbGlobeViewAnalysisProvider()
{

}
CGlbGlobeViewAnalysisProvider::~CGlbGlobeViewAnalysisProvider()
{

}
CGlbGlobeObject* CGlbGlobeViewAnalysisProvider::Create()
{
	return new CGlbGlobeViewAnalysis();
}

CGlbGlobeComplexObjectProvider::CGlbGlobeComplexObjectProvider()
{

}
CGlbGlobeComplexObjectProvider::~CGlbGlobeComplexObjectProvider()
{

}
CGlbGlobeObject* CGlbGlobeComplexObjectProvider::Create()
{
	return new CGlbGlobeCompositeObject();
}

CGlbGlobeDrillProvider::CGlbGlobeDrillProvider()
{
}

CGlbGlobeDrillProvider::~CGlbGlobeDrillProvider()
{
}
CGlbGlobeObject* CGlbGlobeDrillProvider::Create()
{
	return new CGlbGlobeDrill();
}

CGlbGlobeSectionProvider::CGlbGlobeSectionProvider()
{
}

CGlbGlobeSectionProvider::~CGlbGlobeSectionProvider()
{
}
CGlbGlobeObject* CGlbGlobeSectionProvider::Create()
{
	return new CGlbGlobeSection();
}


CGlbGlobeDigHoleProvider::CGlbGlobeDigHoleProvider()
{

}

CGlbGlobeDigHoleProvider::~CGlbGlobeDigHoleProvider()
{

}

CGlbGlobeObject* CGlbGlobeDigHoleProvider::Create()
{
	return new CGlbGlobeDigHole();
}

CGlbGlobeDynamicProvider::CGlbGlobeDynamicProvider()
{

}

CGlbGlobeDynamicProvider::~CGlbGlobeDynamicProvider()
{

}

CGlbGlobeObject* CGlbGlobeDynamicProvider::Create()
{
	return new CGlbGlobeDynamicObject();
}

CGlbGlobeModifyTerrainProvider::CGlbGlobeModifyTerrainProvider()
{

}

CGlbGlobeModifyTerrainProvider::~CGlbGlobeModifyTerrainProvider()
{

}

CGlbGlobeObject* CGlbGlobeModifyTerrainProvider::Create()
{
	return new CGlbGlobeTerrainModify();
}

CGlbGlobeVideoProvider::CGlbGlobeVideoProvider()
{

}

CGlbGlobeVideoProvider::~CGlbGlobeVideoProvider()
{

}

CGlbGlobeObject* CGlbGlobeVideoProvider::Create()
{
	return new CGlbGlobeVideo();
}

CGlbGlobeNetworkProvider::CGlbGlobeNetworkProvider()
{

}

CGlbGlobeNetworkProvider::~CGlbGlobeNetworkProvider()
{

}

CGlbGlobeObject* CGlbGlobeNetworkProvider::Create()
{
	return new CGlbGlobeNetworkObject();
}

CGlbGlobeCoordinateAxisProvider::CGlbGlobeCoordinateAxisProvider()
{

}

CGlbGlobeCoordinateAxisProvider::~CGlbGlobeCoordinateAxisProvider()
{

}

CGlbGlobeObject* CGlbGlobeCoordinateAxisProvider::Create()
{
	return new CGlbGlobeCoordinateAxisObject();
}

CGlbGlobeGridProvider::CGlbGlobeGridProvider()
{

}
CGlbGlobeGridProvider::~CGlbGlobeGridProvider()
{

}
CGlbGlobeObject* CGlbGlobeGridProvider::Create()
{
	return new CGlbGlobeGridObject();
}

CGlbGlobeVirtualDrillProvider::CGlbGlobeVirtualDrillProvider()
{
}

CGlbGlobeVirtualDrillProvider::~CGlbGlobeVirtualDrillProvider()
{
}

CGlbGlobeObject* CGlbGlobeVirtualDrillProvider::Create()
{
	return new CGlbGlobeVirtualDrillObject();
}

CGlbGlobeVirtualSectionProvider::CGlbGlobeVirtualSectionProvider()
{

}
CGlbGlobeVirtualSectionProvider::~CGlbGlobeVirtualSectionProvider()
{

}
CGlbGlobeObject* CGlbGlobeVirtualSectionProvider::Create()
{
	return new CGlbGlobeVirtualSectionObject();
}