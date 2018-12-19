#pragma once
#include "IGlbDataEngine.h"
#include "GlbTSDataSource.h"
class CGlbTSRasterDataset :public IGlbRasterDataset
{
protected:
	CGlbTSRasterDataset(void);
	~CGlbTSRasterDataset(void);
public:
	static CGlbTSRasterDataset* Open(JsonRasterDataset* dsetinfo,CGlbTSDataSource* ds);
public:
	glbBool                QueryInterface(const glbWChar *riid, void **ppvoid);
	GlbDatasetTypeEnum     GetType();
	const glbWChar*        GetName();
	const glbWChar*        GetAlias();
	const IGlbDataSource*  GetDataSource();
	const IGlbDataset*     GetParentDataset();
	const GlbSubDatasetRoleEnum GetSubRole();
	glbWChar*              GetLastError();
	const glbWChar*	       GetSRS();
	glbBool                AlterSRS(const glbWChar* srs);
	const CGlbExtent*      GetExtent() ;
	glbBool                HasZ();
	glbBool                HasM();
	glbBool                GetOrigin(glbDouble* originX, glbDouble* originY);
	glbBool                GetSize(glbInt32 *columns, glbInt32 *rows);
	glbBool                GetRS(glbDouble *resX, glbDouble *resY);
	GlbPixelTypeEnum       GetPixelType();
	GlbPixelLayoutTypeEnum GetPixelLayout();
	const glbWChar*        GetCompressType();
	const glbWChar*        GetFormat();
	glbBool                GetNoData(glbDouble *noDataVal);
	glbBool                AlterNoData(glbDouble noDataVal);
	glbInt32               GetBandCount();
	const IGlbRasterBand*  GetBand(glbInt32 bandIndex);
	glbBool                GetBlockSize(glbInt32* blockSizeX,glbInt32* blockSizeY);
	CGlbPixelBlock*        CreatePixelBlock(glbInt32 columns, glbInt32 rows);
	glbBool                Read(glbInt32 startColum, glbInt32 startRow, CGlbPixelBlock *pPixelBlock);
	glbBool                ReadEx(const CGlbExtent *destExtent, CGlbPixelBlock *pPixelBlock,GlbRasterResampleTypeEnum rspType);
	glbBool                Write(glbInt32 startColum, glbInt32 startRow, CGlbPixelBlock *pPixelBlock);
	glbBool                HasPyramid();
	glbBool                BuildPyramid(
		                                   glbInt32      srcCol,
		                                   glbInt32      srcRow,
		                                   glbInt32      srcCols,
		                                   glbInt32      srcRows,
										   GlbRasterResampleTypeEnum rspType,
		                                   glbInt32      tileSizeX, 
		                                   glbInt32      tileSizeY, 										   
										   glbDouble     lzts        = 36.0, 
								           glbBool       isGlobe     =true, 
										   glbBool       isShareEdge = false, 
								           glbBool       isReplace   =false,
										   glbDouble     rpValue     =0.0, 
										   IGlbProgress* progress    =0);
	glbBool                  DeletePyramid();
	GlbPixelLayoutTypeEnum   GetPyramidPixelLayout();
	GlbPixelTypeEnum         GetPyramidPixelType();
	glbDouble                GetPyramidLZTS();
	glbBool                  IsPyramidShareEdge();
	glbBool                  GetPyramidLevels(glbInt32* minLevel, glbInt32* maxLevel);
	glbBool                  GetPyramidLevelRS(glbInt32 level, 
		                                       glbDouble *resolutionX, 
											   glbDouble *resolutionY);
	glbBool                  GetPyramidTileIndexes(glbInt32  iLevel,
		                                           glbInt32& startTileColumn,
												   glbInt32& startTileRow,
		                                           glbInt32& endTileColumn,
												   glbInt32& endTileRow);
	glbBool                  GetExtentCoverTiles(const CGlbExtent* extent, 
		                                         glbInt32 iLevel,
												 glbInt32& startTileColumn, 
												 glbInt32& startTileRow,
		                                         glbInt32& endTileColumn, 
												 glbInt32& endTileRow);
	glbBool                  GetTilesCoverExtent(glbInt32 level,
		                                         glbInt32 startTileColumn, 
												 glbInt32 startTileRow,
												 glbInt32 endTileColumn, 
		                                         glbInt32 endTileRow,
												 CGlbExtent* extent);
	glbBool                  GetLevelExtent(glbInt32 ilevel, CGlbExtent*);
	glbBool                  GetPyramidBlockSize(glbInt32* blockSizeX, glbInt32* blockSizeY);
	CGlbPixelBlock*          CreatePyramidPixelBlock();
	glbBool                  ReadPyramidByTile(glbInt32 level, 
		                                       glbInt32 tileColumn,
	                                           glbInt32 tileRow, 
											   CGlbPixelBlock *pPixelBlock);
	glbBool                  WritePyramidByTile(glbInt32 level, 
		                                        glbInt32 tileColumn,
	                                            glbInt32 tileRow, 
												CGlbPixelBlock *pPixelBlock);
private:
	glbref_ptr<CGlbTSDataSource>   mpr_ds;    ///<数据源
	CGlbString                     mpr_url;   ///<目录
	CGlbString                     mpr_name;  ///<数据集名
	CGlbWString                    mpr_wname;  ///<数据集名
	CGlbWString                    mpr_alias; ///<数据集别名
	CGlbWString                    mpr_srs;   ///<坐标信息
	glbref_ptr<CGlbExtent>         mpr_extent;///<外包
	GlbPixelTypeEnum               mpr_pixeltype;///<像素类型
	GlbPixelLayoutTypeEnum         mpr_pixellayout;///<像素布局类型
	glbInt32                       mpr_bands;      ///<波段数目
	glbDouble                      mpr_lzts;       ///<LZTS
	glbBool                        mpr_shareEdge;
	glbInt32                       mpr_minlevel;
	glbInt32                       mpr_maxlevel;
	glbInt32                       mpr_tileSizeX;
	glbInt32                       mpr_tileSizeY;
	glbBool                        mpr_isGlobe;	
	GlbCompressTypeEnum            mpr_compressType;
};

