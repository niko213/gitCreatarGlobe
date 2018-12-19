#pragma once
#include "IGlbDataEngine.h"
#include "GlbFSDataSource.h"
class CGlbFSObjectClass:public IGlbObjectClass
{
protected:
	CGlbFSObjectClass(void);	
public:
	~CGlbFSObjectClass(void);
	static CGlbFSObjectClass* Open(JsonDataset* dset,CGlbFSDataSource* ds,IGlbDataset* parent,GlbSubDatasetRoleEnum role);
	glbBool QueryInterface(const glbWChar *riid,void **ppvoid);
	GlbDatasetTypeEnum GetType();
	const glbWChar* GetName();
	const glbWChar* GetAlias();
	const IGlbDataSource* GetDataSource();
	const IGlbDataset* GetParentDataset();
	const GlbSubDatasetRoleEnum GetSubRole();
	glbWChar* GetLastError();

	glbBool AddField(CGlbField* field);
	glbBool DeleteField(const glbWChar * fname);
	CGlbFields* GetFields();
	CGlbIndexes* GetIndexes();
	glbBool AddIndex(CGlbIndex* index);
	glbBool DeleteIndex(const glbWChar* iname);
	glbInt32 GetObjectCount();
	CGlbObject* GetObject(glbInt32 oid);
	glbBool UpdateObject(CGlbObject* obj);
	glbBool DeleteObject(glbInt32 oid);
	glbBool DeleteObjects(CGlbQueryFilter* queryfilter);
	IGlbObjectCursor* Query(CGlbQueryFilter* queryfilter);
	IGlbObjectUpdateCursor*	Update(CGlbQueryFilter* queryfilter);
	IGlbObjectInsertCursor*	Insert();
private:	
	CGlbString                    mpr_url;	
	CGlbWString                   mpr_wname;
	CGlbWString                   mpr_alias;
	GlbDatasetTypeEnum            mpr_type;
	glbref_ptr<CGlbFields>        mpr_fields;
	glbref_ptr<IGlbDataset>       mpr_parent;
	GlbSubDatasetRoleEnum         mpr_role;
public:
	glbref_ptr<CGlbFSDataSource>  mpr_ds;
	CGlbString                    mpr_name;
};

