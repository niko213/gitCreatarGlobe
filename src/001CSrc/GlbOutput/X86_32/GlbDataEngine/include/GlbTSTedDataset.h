#pragma once
#include "IGlbDataEngine.h"
#include "GlbTSDataSource.h"
#include "GlbJSON.h"
class CGlbTSTedDataset:public IGlbTerrainDataset
{
	typedef struct _GlbLevelInfo
	{
		glbDouble OriginX;///<原点x坐标
		glbDouble OriginY;///<原点y坐标
		glbInt32 Rows;///<分块行数
		glbInt32 Cols;///<分块列数
		glbInt32 startRow;
		glbInt32 endRow;
		glbInt32 startCol;
		glbInt32 endCol;
	} GlbLevelInfo;//级别的外包信息

protected:
	CGlbTSTedDataset(void);
	~CGlbTSTedDataset(void);
public:
	static CGlbTSTedDataset* Open(JsonTedDataset* dset,CGlbTSDataSource* ds);
public:
	glbBool            QueryInterface(const glbWChar *riid, void **ppvoid);
    GlbDatasetTypeEnum GetType();
	const glbWChar*    GetName();
	const glbWChar*    GetAlias();
	const IGlbDataSource* GetDataSource();
	const IGlbDataset*    GetParentDataset();
	const GlbSubDatasetRoleEnum GetSubRole();
	glbWChar*             GetLastError();
    const glbWChar*	      GetSRS();
	glbBool               AlterSRS(const glbWChar* srs);
	const CGlbExtent*     GetExtent() ;
	glbBool               HasZ();
	glbBool               HasM();
	

	glbBool          GetLZTS(glbDouble& lzts);
	glbBool          IsGlobe();
	GlbPixelTypeEnum       GetDomPixelType();
	GlbPixelTypeEnum       GetDemPixelType();
	GlbPixelLayoutTypeEnum GetDomPixelLayout();
	GlbPixelLayoutTypeEnum GetDemPixelLayout();
	glbBool                GetDomBlockSize(glbInt32& blockSizeX, glbInt32& blockSizeY);
	glbBool                GetDemBlockSize(glbInt32& blockSizeX, glbInt32& blockSizeY);
	glbInt32 GetDomBandCount();
	glbInt32 GetDemBandCount();
	glbBool  GetDomLevels(glbInt32& maxLevel,glbInt32& minLevel);
	glbBool  GetDemLevels(glbInt32& maxLevel,glbInt32& minLevel);
	glbBool  GetDomLevelExtent(glbInt32 iLevel, CGlbExtent* ext);
	glbBool  GetDemLevelExtent(glbInt32 iLevel, CGlbExtent* ext);
	glbBool  GetDomTileIndexes(glbInt32 level, 
		                       glbInt32& startTileColumn,  
							   glbInt32& startTileRow, 
		                       glbInt32& endTileColumn,  
							   glbInt32&  endTileRow);
	glbBool GetDemTileIndexes(glbInt32 level, 
		                      glbInt32& startTileColumn,
							  glbInt32& startTileRow, 
		                      glbInt32& endTileColumn,
							  glbInt32&  endTileRow);
	glbBool GetDomExtentCoverTiles(const CGlbExtent* extent, 
		                           glbInt32          level,
								   glbInt32&         startTileColumn, 
								   glbInt32&         startTileRow,
		                           glbInt32&         endTileColumn, 
								   glbInt32&         endTileRow);
	glbBool GetDemExtentCoverTiles(const CGlbExtent* extent,
		                          glbInt32           level,
								  glbInt32&          startTileColumn, 
								  glbInt32&          startTileRow,
		                          glbInt32&          endTileColumn, 
								  glbInt32&          endTileRow);
    glbBool GetDomTilesCoverExtent(glbInt32 level,
		                           glbInt32 startTileColumn, 
								   glbInt32 startTileRow,
		                           glbInt32 endTileColumn, 
								   glbInt32 endTileRow, 
								   CGlbExtent* extent);
	glbBool GetDemTilesCoverExtent(glbInt32 level,
		                           glbInt32 startTileColumn, 
								   glbInt32 startTileRow,
		                           glbInt32 endTileColumn,
								   glbInt32 endTileRow,
								   CGlbExtent* extent);
	CGlbPixelBlock* CreateDemPixelBlock();
	CGlbPixelBlock* CreateDomPixelBlock();
	glbBool ReadDom(glbInt32 level, glbInt32 tileCol,glbInt32 tileRow, CGlbPixelBlock* pixelBlock);
	glbBool ReadDem( glbInt32 level, glbInt32 tileCol,glbInt32 tileRow, CGlbPixelBlock* pixelBlock);
	glbBool ReadDem(glbDouble lonOrX,glbDouble latOrY, CGlbPixelBlock* pixelBlock);
	glbBool WriteDom(glbInt32        level,
		             glbInt32        tileCol,
					 glbInt32        tileRow,
					 GlbTileNeighbor& neighbors,
					 CGlbPixelBlock* pixelBlock);
	glbBool WriteDem(glbInt32 level,
		             glbInt32 tileCol,
					 glbInt32 tileRow,
					 GlbTileNeighbor& neighbors,
					 CGlbPixelBlock* pixelBlock);
	glbBool MergerGlobeDem();
	glbBool ImportDomDataset(IGlbRasterDataset* dataset, 
		                     glbInt32           minLevel,
						     glbInt32           maxLevel,
							 IGlbProgress* progress=NULL);
	glbBool ImportDemDataset(IGlbRasterDataset* dataset, 
		                     glbInt32           minLevel,
							 glbInt32           maxLevel,
							 IGlbProgress* progress=NULL);
private:
	glbref_ptr<CGlbTSDataSource> mpr_ds;
	CGlbString                    mpr_url;
	CGlbString                    mpr_name;
	CGlbWString                   mpr_wname;
	CGlbWString                   mpr_alias;
	CGlbWString                   mpr_srs;
	glbref_ptr<CGlbExtent>        mpr_extent;
	glbDouble                     mpr_lzts;
	glbBool                       mpr_isGlobe;
    
	//dom
	GlbCompressTypeEnum           mpr_domCompress;
	glbInt32                      mpr_domMinlevel;
	glbInt32                      mpr_domMaxlevel;
	//dem
	GlbCompressTypeEnum           mpr_demCompress;
	glbInt32                      mpr_demMinlevel;
	glbInt32                      mpr_demMaxlevel;

	// domLevelInfo , demLevelInfo
	std::map<glbInt32/*level*/,GlbLevelInfo*> mpr_domLevelInfo;
	std::map<glbInt32/*level*/,GlbLevelInfo*> mpr_demLevelInfo;		
};

