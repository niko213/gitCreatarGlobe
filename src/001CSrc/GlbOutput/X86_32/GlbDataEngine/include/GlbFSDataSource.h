#pragma once
#include "IGlbDataEngine.h"
#include "GlbDataSourceFactory.h"
#include "GlbWString.h"
#include "GlbString.h"
#include "GlbJSON.h"
class CGlbFSDataSource : public IGlbFeatureDataSource
{
protected:
	CGlbFSDataSource(void);
public:
	static CGlbFSDataSource* Open(const glbWChar* url, 
		                          const glbWChar* user,
		                          const glbWChar* password);
	~CGlbFSDataSource(void);
	////IGlbDataSource
	glbInt32             GetCount();
	IGlbDataset*         GetDataset(glbInt32 datasetIndex);
	IGlbDataset*         GetDatasetByName(const glbWChar* datasetName);
	glbBool              DeleteDataset (const glbWChar * datasetName);
	const glbWChar*	     GetAlias();
	glbBool              SetAlias(const glbWChar* alias);
	const glbWChar*	     GetProviderName();
	glbInt32             GetID() ;
	glbBool              SetID(glbInt32 id);
	glbWChar*            GetLastError();
	////IGlbUnknown
	glbBool              QueryInterface(const glbWChar *riid,void **ppvoid);
	////IGlbFeatureDataSource
	IGlbDataset* CreateDataset(const glbWChar* name ,
		const glbWChar* alias,
		GlbDatasetTypeEnum type,
		const glbWChar* srs,
		glbBool hasZ, 
		glbBool hasM);
	glbBool ExecuteSQL(const glbWChar* sqlstr );
	IGlbObjectCursor* Query(const glbWChar* querystr);

public:
	glbInt32    userid;
	//CGlbString  token;
	CGlbString   GetToken()
	{
		CGlbDataSourceFactory* dsf = CGlbDataSourceFactory::GetInstance();
		map<long,CGlbString>::iterator itr =  dsf->mpr_webtokens.find(userid);
		if(itr != dsf->mpr_webtokens.end())
			return itr->second;
		return "";
	}
	void         SetToken(CGlbString token)
	{
		CGlbDataSourceFactory* dsf = CGlbDataSourceFactory::GetInstance();
		dsf->mpr_webtokens[userid]=token;
	}
	CGlbString  url;
private:
	glbInt32             mpr_id;
	CGlbWString          mpr_alias;
	vector<JsonDataset*> mpr_dsets;
};

