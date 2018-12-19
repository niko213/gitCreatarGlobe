#pragma once
#include "IGlbDataEngine.h"
#include "GlbQueryFilter.h"
#include "GlbFSFeatureclass.h"

class CGlbFSFeatureCursor : public IGlbFeatureCursor
{
protected:
	CGlbFSFeatureCursor(void);
public:
	~CGlbFSFeatureCursor(void);
	static CGlbFSFeatureCursor* Open(CGlbFSDataSource* ds,IGlbFeatureClass* cls,CGlbQueryFilter* filter);
	glbInt32                GetCount();
	CGlbFields*             GetFields();
	const IGlbFeatureClass* GetClass();
	glbBool                 Requery();
	glbBool                 MoveNext();
	CGlbFeature*            GetFeature();
	glbWChar*               GetLastError();
private:
	glbref_ptr<IGlbFeatureClass> mpr_cls;
	glbref_ptr<CGlbFSDataSource>   mpr_ds;
	glbref_ptr<CGlbFields>         mpr_fields;
	CGlbString                     mpr_fieldstr;
	CGlbString                     mpr_wherestr;
	CGlbString                     mpr_extentstr;

	glbInt32                       mpr_totalnum;
	glbInt32                       mpr_hasQNum;
	glbInt32                       mpr_index;
	std::vector<CGlbFeature*>      mpr_features;
	glbInt32                       mpr_rowno;
};

