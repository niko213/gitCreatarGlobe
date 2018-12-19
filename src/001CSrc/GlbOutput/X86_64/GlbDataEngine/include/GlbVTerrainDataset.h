#pragma once
#include "IGlbDataEngine.h"

/*
	@brief 虚拟平面地形数据集
*/
class GLBDATAENGINE_API CGlbVTerrainDataset:public IGlbTerrainDataset
{
public:
	CGlbVTerrainDataset(CGlbExtent* ext);
	~CGlbVTerrainDataset(void);

	glbBool QueryInterface( const glbWChar *riid, void **ppvoid );
	GlbDatasetTypeEnum GetType();
	const glbWChar* GetName();
	const glbWChar* GetAlias();
	const IGlbDataSource* GetDataSource();
	const IGlbDataset* GetParentDataset();
	const GlbSubDatasetRoleEnum GetSubRole();
	glbWChar* GetLastError();
	const glbWChar* GetSRS();
	glbBool AlterSRS(const glbWChar* srs);
	const  CGlbExtent* GetExtent();
	glbBool HasZ();
	glbBool HasM();
	glbBool GetLZTS(glbDouble& lzts);
	glbBool IsGlobe();
	GlbPixelTypeEnum GetDomPixelType();
	GlbPixelTypeEnum GetDemPixelType();
	GlbPixelLayoutTypeEnum GetDomPixelLayout();
	GlbPixelLayoutTypeEnum GetDemPixelLayout();
	glbBool GetDomBlockSize(glbInt32& blockSizeX, glbInt32& blockSizeY);
	glbBool GetDemBlockSize(glbInt32& blockSizeX, glbInt32& blockSizeY);
	glbInt32 GetDomBandCount();
	glbInt32 GetDemBandCount();
	glbBool GetDomLevels(glbInt32& maxLevel,glbInt32& minLevel);
	glbBool GetDemLevels(glbInt32& maxLevel,glbInt32& minLevel);	
	glbBool GetDomLevelExtent(glbInt32 iLevel, CGlbExtent* ext);
	glbBool GetDemLevelExtent(glbInt32 iLevel, CGlbExtent* ext);
	glbBool GetDomTileIndexes(glbInt32 level, 
		                      glbInt32& startTileCol,  
							  glbInt32& startTileRow, 
		                      glbInt32& endTileCol,  
							  glbInt32& endTileRow);
	glbBool GetDemTileIndexes(glbInt32 level, 
		                      glbInt32& startTileCol,
							  glbInt32& startTileRow, 
		                      glbInt32& endTileCol,  
							  glbInt32& endTileRow);
	glbBool GetDomExtentCoverTiles(const CGlbExtent* extent, 
								   glbInt32  level,
								   glbInt32& startTileCol,
								   glbInt32& startTileRow,
		                           glbInt32& endTileCol,
								   glbInt32& endTileRow);
	glbBool GetDemExtentCoverTiles(const CGlbExtent* extent, 
		                                  glbInt32  level,
										  glbInt32& startTileCol, 
										  glbInt32& startTileRow,
		                                  glbInt32& endTileCol, 
										  glbInt32& endTileRow);
	glbBool GetDomTilesCoverExtent(glbInt32 level,
		                                   glbInt32 startTileCol, 
										   glbInt32 startTileRow,
		                                   glbInt32 endTileCol, 
										   glbInt32 endTileRow, 
										   CGlbExtent* extent);	
	glbBool GetDemTilesCoverExtent(glbInt32 level,
		                                   glbInt32 startTileCol, 
										   glbInt32 startTileRow,
		                                   glbInt32 endTileCol, 
										   glbInt32 endTileRow, 
										   CGlbExtent* extent);
	CGlbPixelBlock* CreateDemPixelBlock();
	CGlbPixelBlock* CreateDomPixelBlock();
	glbBool ReadDom(glbInt32 level, glbInt32 tileCol,glbInt32 tileRow, CGlbPixelBlock* pixelBlock);
	glbBool ReadDem(glbInt32 level, glbInt32 tileCol,glbInt32 tileRow, CGlbPixelBlock* pixelBlock);
	glbBool ReadDem(glbDouble lonOrX,glbDouble latOrY, CGlbPixelBlock* pixelBlock);
	glbBool WriteDom(glbInt32 level, 
		                     glbInt32 tileCol, 
							 glbInt32 tileRow, 
							 GlbTileNeighbor& neighbors, 
							 CGlbPixelBlock* pixelBlock);
	glbBool WriteDem(glbInt32 level, 
		                     glbInt32 tileCol,
							 glbInt32 tileRow, 
							 GlbTileNeighbor& neighbors,
							 CGlbPixelBlock*  pixelBlock);	
	glbBool MergerGlobeDem();
	glbBool ImportDomDataset(IGlbRasterDataset* dataset,
		                             glbInt32 minLevel,
								     glbInt32 maxLevel,
		                             IGlbProgress* progress=NULL);
	glbBool ImportDemDataset(IGlbRasterDataset* dataset,
		                             glbInt32 minLevel,
								     glbInt32 maxLevel,
		                             IGlbProgress* progress=NULL);
private:
	glbref_ptr<CGlbExtent> mpr_ext;
	glbDouble              mpr_lzts;
};

