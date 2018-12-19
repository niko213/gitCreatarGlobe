#pragma once
#include "IGlbDataEngine.h"
#include "GlbDataSourceFactory.h"
#include "GlbJSON.h"
class CGlbTSDataSource : public IGlbRasterDataSource
{
protected:
	CGlbTSDataSource(void);
public:
	static CGlbTSDataSource* Open(const glbWChar* url, 
		                          const glbWChar* user,
		                          const glbWChar* password);
	~CGlbTSDataSource(void);
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
	IGlbRasterDataset*  GetRasterDataset( const glbWChar * datasetName);
	IGlbTerrainDataset* GetTerrainDataset(const glbWChar* terrainDatasetName);
	IGlbRasterDataset*  CreateRasterDataset(const glbWChar              *name,
		                                   const glbWChar              *alias,
		                                   glbInt32                    nBands,
		                                   GlbPixelTypeEnum            pixelType,
		                                   glbInt32                    columns,
		                                   glbInt32                    rows,
		                                   glbDouble                   orgX    =0,
		                                   glbDouble                   orgY    =0,
		                                   glbDouble                   rsX     =1,
		                                   glbDouble                   rsY     =1,
		                                   const glbWChar*             format  =L"GTiff",
		                                   const glbWChar*             SRS     =NULL,
		                                   const GlbRasterStorageDef   *storeDef=NULL);
	IGlbTerrainDataset* CreateTerrainDataset(const glbWChar* name,
		                                     const glbWChar* alias,
		                                     bool isGlobe=true,
		                                     glbDouble              lzts=36,
		                                     const glbWChar*        SRS=NULL,
		                                     GlbCompressTypeEnum domCompress=GLB_COMPRESSTYPE_JPEG,
		                                     GlbCompressTypeEnum demCompress=GLB_COMPRESSTYPE_ZLIB);
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

