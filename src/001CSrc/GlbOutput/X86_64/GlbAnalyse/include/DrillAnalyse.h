// GlbDrillAnalyse.h

#pragma once

#include "IGlbDataEngine.h"
#include "GlbFeature.h"
#include "GlbTin.h"
#include "GlbDrill.h"
#include "IGlbGeometry.h"

#ifdef GLBDRILLANALYSIS
#define DRILL_ANALYSIS_EXPORT __declspec(dllexport)
#else
#define DRILL_ANALYSIS_EXPORT __declspec(dllimport)
#endif
namespace GlbAnalysis
{
	class DRILL_ANALYSIS_EXPORT GlbDrillAnalyse
	{
		// TODO: Add your methods for this class here.
	public:
		bool CreateDrill(double pts[], CGlbWString drill_ids[], int pt_num, IGlbDataset* tin_ds, IGlbDrillDataset* drill_ds, bool isGlobeMode);
		bool CreateDrill(double x, double y, double z, CGlbWString drill_id, IGlbDataset* tin_ds, IGlbDrillDataset* drill_ds, bool isGlobeMode = true);
	};
}
