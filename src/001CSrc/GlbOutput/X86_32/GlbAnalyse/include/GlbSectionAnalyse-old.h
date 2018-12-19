// GlbSectionAnalyse.h

#pragma once

#include "IGlbDataEngine.h"
#include "GlbReference.h"
#include "GlbSectionAnalyseExport.h"

namespace GlbAnalysis
{
	class GLBDRILLANALYSE_API CGlbSectionAnalyse :public CGlbReference	
	{
		// TODO: Add your methods for this class here.
	public:
		static bool CreateSection(double pts[] , int pt_num, CGlbWString sec_id, IGlbDataset* tin_ds, IGlbSectionDataset* sec_ds, bool isGlobeMode = true);

		static bool CreateSection(double pts[] , int pt_num, IGlbDataset* tin_ds, const glbWChar* fieldName, IGlbGeometry** secGeo, glbWChar** secInfo);		
	};
}