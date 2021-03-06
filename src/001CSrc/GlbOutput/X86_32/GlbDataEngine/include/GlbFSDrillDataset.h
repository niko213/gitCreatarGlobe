#pragma once
#include "IGlbDataEngine.h"
#include "GlbFSDataSource.h"

class CGlbFSDrillDataset : public IGlbDrillDataset
{
protected:
	CGlbFSDrillDataset(void);
public:
	~CGlbFSDrillDataset(void);
	static CGlbFSDrillDataset* Open(JsonDrillDataset* dset,CGlbFSDataSource* ds);
	glbBool QueryInterface(const glbWChar *riid,void **ppvoid);
	GlbDatasetTypeEnum    GetType();
	const glbWChar*       GetName();
	const glbWChar*       GetAlias();
	const IGlbDataSource* GetDataSource();
	const IGlbDataset*    GetParentDataset();
	const GlbSubDatasetRoleEnum GetSubRole();
	glbWChar*             GetLastError();
	const glbWChar*       GetSRS();
	glbBool	              AlterSRS(const glbWChar* srs);
	const                 CGlbExtent* GetExtent();
	glbBool               HasZ();
	glbBool               HasM();
	glbBool                  AddField(CGlbField* field);
	glbBool                  DeleteField(const glbWChar * fname);
	const CGlbFields*        GetFields();	
	const CGlbIndexes*       GetIndexes();	
	glbBool                  AddIndex(CGlbIndex* index);	
	glbBool	                 DeleteIndex(const glbWChar* iname);	
	GlbSpatialIndexInfo*     GetSpatialIndexInfo();	
	glbBool                  BuildSpatialIndex(GlbSpatialIndexInfo * sidxInfo);	
	glbBool                  DeleteSpatialIndex();	
	glbInt32                 GetFeatureCount();	
	CGlbFeature*             GetFeature(glbInt32 oid);	
	glbBool                  UpdateFeature(CGlbFeature* feature);	
	glbBool                  DeleteFeature(glbInt32 oid);	
	glbBool                  DeleteFeatures(CGlbQueryFilter* queryfilter);	
	IGlbFeatureCursor*       Query(CGlbQueryFilter* queryfilter);	
	IGlbFeatureUpdateCursor* Update(CGlbQueryFilter* queryfilter);	
	IGlbFeatureInsertCursor* Insert();	
	GlbGeometryTypeEnum      GetGeometryType();	
	glbBool                  AddGeoField(GlbGeometryClassEnum subgeocls, CGlbField* field);	
	glbBool                  DeleteGeoField(GlbGeometryClassEnum subgeocls, const glbWChar* fname);	
	const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum subgeocls);	
	glbBool                  GetGeometry(glbInt32 oid,IGlbGeometry** geo,glbInt32 level = 0);

	IGlbObjectClass* GetSkewClass();
	IGlbFeatureClass* GetStratumClass();
	glbInt32 GetSampleClassCount();
	IGlbObjectClass* GetSampleClass(glbInt32 idx);
	IGlbObjectClass* GetSampleClassByName(const glbWChar* name);
	IGlbObjectClass* CreateSampleClass(const glbWChar* name, const glbWChar* alias);
	glbBool DeleteSampleClass(const glbWChar* name);
private:	
	CGlbString                    mpr_url;	
	CGlbWString                   mpr_wname;
	CGlbWString                   mpr_alias;
	CGlbWString                   mpr_srs;
	glbref_ptr<CGlbExtent>        mpr_extent;
	GlbDatasetTypeEnum            mpr_type;
	GlbGeometryTypeEnum           mpr_geotype;
	glbBool                       mpr_hasz;
	glbBool                       mpr_hasm;
	glbref_ptr<CGlbFields>        mpr_fields;
	JsonDrillDataset*             mpr_jsondset;
public:
	glbref_ptr<CGlbFSDataSource>  mpr_ds;
	CGlbString                    mpr_name;
};

