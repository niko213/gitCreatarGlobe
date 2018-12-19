#include "IGlbDataEngine.h"
#include "GlbDrill.h"
#include "GlbLine.h"

#ifdef GLBDRILLMODELING_EXPORTS
#define DRILLMODELING_EXPORT __declspec(dllexport)
#else
#define DRILLMODELING_EXPORT __declspec(dllimport)
#endif

namespace GlbAnalysis
{
	class DRILLMODELING_EXPORT DrillModelingTool
	{
		// TODO: Add your methods for this class here.
	public:
		virtual bool SetPara(bool bSimpleModel,double cellsize,double mindisXY,double mindisZ, bool bExtendVirtual, bool isGlobeMode = true)=0;
		virtual bool DoDrillModeling(CGlbLine* border, IGlbDrillDataset* drill_ds, IGlbDataset* tin_ds, bool isGlobeMode = true)=0;
		virtual bool DoDrillModeling(CGlbLine* border, CGlbDrill** drills, long drill_num, IGlbDataset* tin_ds, bool isGlobeMode = true)=0;
	};

	class DRILLMODELING_EXPORT DrillModelingFactory
	{
	public:
		static DrillModelingTool* GetDrillModelingTool();
	};
}
