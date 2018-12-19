#pragma once
#include "IGlbDataEngine.h"
#include "GlbQueryFilter.h"
#include "GlbFSObjectClass.h"
class CGlbFSObjectCursor : public IGlbObjectCursor
{
protected:
	CGlbFSObjectCursor(void);
public:
	~CGlbFSObjectCursor(void);
	static CGlbFSObjectCursor* Open(CGlbFSDataSource* ds,CGlbFSObjectClass* cls,CGlbQueryFilter* filter);
	CGlbFields*      GetFields();
	IGlbObjectClass* GetClass();
	glbBool          Requery();
	glbBool          MoveNext();
	glbInt32         GetCount();
	CGlbObject*      GetObject();
	glbWChar*        GetLastError();
private:
	glbref_ptr<CGlbFSObjectClass> mpr_cls;
	glbref_ptr<CGlbFSDataSource>   mpr_ds;
	glbref_ptr<CGlbFields>         mpr_fields;
	CGlbString                     mpr_fieldstr;
	CGlbString                     mpr_wherestr;
	CGlbString                     mpr_extentstr;

	glbInt32                       mpr_totalnum;
	glbInt32                       mpr_hasQNum;
	glbInt32                       mpr_index;
	std::vector<CGlbObject*>       mpr_objects;
	glbInt32                       mpr_rowno;
};

