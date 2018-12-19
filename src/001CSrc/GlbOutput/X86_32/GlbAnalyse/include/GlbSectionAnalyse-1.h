// GlbSectionAnalyse.h

#pragma once

#include "IGlbDataEngine.h"
#include "GlbReference.h"
#include "GlbSectionAnalyseExport.h"
#include "GlbLine.h"

struct sectionData
{
	sectionData()
	{
		m_oid = -1;
		m_rockTexture = NULL;
	}
	long m_oid;
	glbInt32 m_color; // 地层颜色
	glbWChar* m_rockTexture; // 地层纹理
	std::vector<GlbPoint3D> m_sectionPoint;
};
struct sectionSeg
{
	GlbPoint3D m_setP[2];
};

namespace GlbAnalysis
{
	class GLBDRILLANALYSE_API CGlbSectionAnalyse :public CGlbReference	
	{
		// TODO: Add your methods for this class here.
	public:
		static bool CreateSection(double pts[] , int pt_num, CGlbWString sec_id, IGlbDataset* tin_ds, IGlbSectionDataset* sec_ds, bool isGlobeMode = true);
		// 最新算法-整体求折剖面算法
		static bool CreateSection(double pts[] , int pt_num, IGlbDataset* tin_ds, const glbWChar* fieldName, IGlbGeometry** secGeo, glbWChar** secInfo);	
		// 分段求剖面算法
		static bool CreateSection2(double pts[] , int pt_num, IGlbDataset* tin_ds, const glbWChar* fieldName, IGlbGeometry** secGeo, glbWChar** secInfo);		

		static bool CreateSectionSeg(const GlbPoint3D& lineP1, const GlbPoint3D& lineP2, const GlbPoint3D triangleP[], GlbPoint3D& intrP1, GlbPoint3D& intrP2);

		static bool segmentsIntr(const GlbPoint3D& a, const GlbPoint3D& b, const GlbPoint3D& c, const GlbPoint3D& d, GlbPoint3D& intrP);

		static bool segmentsIntr2(GlbPoint3D a, GlbPoint3D b, GlbPoint3D c, GlbPoint3D d, double& intr_x, double& intr_y);

		static bool pointInTrangle(double ptsX, double ptsY, double& ptsZ, const GlbPoint3D trangle[]);

		static bool createSectionData(std::vector<sectionData>& sectionDataVect, std::vector<sectionSeg>& sectSegVect);

		static bool CreateGridSection(int x_Num, int y_Num, IGlbDataset* tin_ds,const glbWChar* fieldName,IGlbGeometry** secGeo, glbWChar** secInfo);

		static bool CreateSectionByMultLine(CGlbMultiLine** multLine, IGlbDataset* tin_ds,const glbWChar* fieldName,IGlbGeometry** secGeo, glbWChar** secInfo);
	};
}