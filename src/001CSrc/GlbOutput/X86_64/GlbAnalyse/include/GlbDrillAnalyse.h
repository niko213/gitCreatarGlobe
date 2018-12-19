// GlbDrillAnalyse.h

#pragma once

#include "IGlbDataEngine.h"
#include "GlbReference.h"
#include "GlbDrillAnalyseExport.h"

namespace GlbAnalysis
{
	class GLBDRILLANALYSE_API CGlbDrillAnalyse : public CGlbReference	
	{
		// TODO: Add your methods for this class here.
	public:
		//CGlbDrillAnalyse();
		//~CGlbDrillAnalyse();

		static bool CreateDrill(double pts[], int pt_num, IGlbDataset* tin_ds, IGlbDrillDataset* drill_ds, bool isGlobeMode = true);		
		static bool CreateDrill(double x, double y, double z, CGlbWString drill_id, IGlbDataset* tin_ds, IGlbDrillDataset* drill_ds, bool isGlobeMode = true);		

		static bool CreateDrill(double x, double y, double z,IGlbDataset* tin_ds,const glbWChar* field_name, IGlbGeometry** drillGeo, glbWChar** drillInfo);
		static bool CreateDrill(double x, double y, double z, double depth, IGlbDataset* tin_ds, std::vector<glbInt32>& featureIds, const glbWChar* field_name, IGlbGeometry** drillGeo, glbWChar** drillInfo);
	private:
		static bool pointInTrangle(double ptsX, double ptsY, double& ptsZ, GlbPoint3D trangle[]);
		static bool segmentsIntr(GlbPoint3D a, GlbPoint3D b, GlbPoint3D c, GlbPoint3D d, double& intr_x, double& intr_y);
	};
}
