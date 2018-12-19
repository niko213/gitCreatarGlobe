// GlbSectionAnalyse.h

#pragma once

#include "IGlbDataEngine.h"

namespace GlbAnalysis
{
	class __declspec(dllexport) SectionAnalyse
	{
		// TODO: Add your methods for this class here.
	public:
		bool CreateSection(double pts[] , int pt_num, CGlbWString sec_id, IGlbDataset* tin_ds, IGlbSectionDataset* sec_ds, bool isGlobeMode=true);
	};
}