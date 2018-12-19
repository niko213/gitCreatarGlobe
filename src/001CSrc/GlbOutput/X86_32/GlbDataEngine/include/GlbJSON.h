#pragma once
#include <vector>
#include "json.h"
#include "GlbCommTypes.h"
#include "GlbExtent.h"
#include "GlbDataEngineType.h"
#include "GlbPixelBlock.h"
#include "GlbFields.h"
#include "GlbObject.h"
#include "GlbFeature.h"
#include "IGlbDataEngine.h"

struct JsonLoginInfo
{
	CGlbString isok;
	CGlbString message;
	glbInt32   userid;
	CGlbString token;
};
struct JsonDataset
{
	CGlbString name;
	CGlbString alias;
	GlbDatasetTypeEnum type;
};
struct JsonRasterDataset:JsonDataset
{
	CGlbString    srs;
    CGlbExtent    extent;
    bool          isglobe;
    double        lzts;
    GlbPixelTypeEnum    pixeltype;
    GlbPixelLayoutTypeEnum  pixellayout;
    int                 blocksizex;
    int                 blocksizey;
    GlbCompressTypeEnum compresstype;
    int                   bands;
    int                   minlevel;
    int                   maxlevel;
    bool                  isshareedge;
};
struct JsonGlbRasterInfo
{
	GlbPixelTypeEnum       pixeltype;
	GlbPixelLayoutTypeEnum pixellayout;
    int blocksizex;
    int blocksizey;
    GlbCompressTypeEnum compresstype;
    int bands;
    int minlevel;
    int maxlevel;
    bool isshareedge;
};

struct JsonGlbLevelInfo 
{
	int level;
	double OriginX;///<原点x坐标
	double OriginY;///<原点y坐标
	int Rows;///<分块行数
	int Cols;///<分块列数
	int startRow;
	 int endRow;
	 int startCol;
	 int endCol;
};

struct JsonTedDataset:JsonDataset
{
	CGlbString srs;
    CGlbExtent extent;
    bool       isglobe;
    double     lzts;            
    JsonGlbRasterInfo dominfo;
    JsonGlbRasterInfo deminfo;

	std::vector<JsonGlbLevelInfo> mpr_domLevelInfo;
	std::vector<JsonGlbLevelInfo> mpr_demLevelInfo;
};
struct JsonFeatureclass : JsonDataset
{
	CGlbString srs;
    CGlbExtent extent;
    bool        hasz;
    bool        hasm;
    GlbGeometryTypeEnum geotype;
};
struct JsonDrillDataset:JsonFeatureclass
{
	JsonDataset* skewcls;
	JsonDataset* stratumcls;
	std::vector<JsonDataset*> sampleclses;
	JsonDrillDataset()
	{
		skewcls   = NULL;
		stratumcls= NULL;		
	}
	~JsonDrillDataset()
	{
		if(skewcls)    delete skewcls;
		if(stratumcls) delete stratumcls;
		std::vector<JsonDataset*>::iterator itr   = sampleclses.begin();
		std::vector<JsonDataset*>::iterator itrEnd=sampleclses.end();
		for(itr;itr!=itrEnd;itr++)
			delete (*itr);
	}
};
struct JsonSectionDataset:JsonFeatureclass
{
	JsonDataset* ctrlcls;
	JsonDataset* nodecls;
	JsonDataset* arccls;
	JsonDataset* polycls;
	JsonSectionDataset()
	{
		ctrlcls = NULL;
		nodecls = NULL;
		arccls  = NULL;
		polycls = NULL;
	}
	~JsonSectionDataset()
	{
		if(ctrlcls) delete ctrlcls;
		if(nodecls) delete nodecls;
		if(arccls)  delete arccls;
		if(polycls) delete polycls;
	}
};
struct JsonNetworkDataset:JsonFeatureclass
{
	JsonDataset* nodecls;
	JsonDataset* turncls;
	JsonNetworkDataset()
	{
		nodecls = NULL;
		turncls = NULL;
	}
	~JsonNetworkDataset()
	{
		if(nodecls)delete nodecls;
		if(turncls)delete turncls;
	}
};
struct JsonCapabiliesInfo
{
	CGlbString isok;
	CGlbString message;
	CGlbString token;
	std::vector<JsonDataset*> dsets;

	JsonCapabiliesInfo(){}
	~JsonCapabiliesInfo()
	{
		std::vector<JsonDataset*>::iterator itr = dsets.begin();
		std::vector<JsonDataset*>::iterator itrEnd=dsets.end();
		for(itr;itr!=itrEnd;itr++)
			delete (*itr);
	}
};
struct JsonPixelBlock
{
	CGlbString isok;
	CGlbString message;
	CGlbString token;
	CGlbPixelBlock *pblock;
};
struct JsonFields
{
	CGlbString isok;
	CGlbString message;
	CGlbString token;
	CGlbFields* fields;
};
struct JsonObjects
{
	CGlbString isok;
	CGlbString message;
	CGlbString token;
	glbInt32   totalnum;
	std::vector<CGlbObject*> objects;
};
struct JsonFeatures
{
	CGlbString isok;
	CGlbString message;
	CGlbString token;
	glbInt32   totalnum;
	std::vector<CGlbFeature*> features;	
};
struct JsonGeometry
{
	glbInt32 level;
	IGlbGeometry* geo;
};
struct JsonGeometries
{
	std::vector<JsonGeometry> geoes;
};
class GlbJSON
{
public:
	GlbJSON(void);
	~GlbJSON(void);
public:
	glbBool loginDecode(const CGlbString& rdstr,JsonLoginInfo& login);
	glbBool capabilitiesDecode(const CGlbString& rdstr,JsonCapabiliesInfo& dsets);
	glbBool tileDecode(const CGlbString& rdstr,JsonPixelBlock& jblock,GlbCompressTypeEnum compress);
	glbBool fieldsDecode(const CGlbString& rdstr,JsonFields& jsonfields);
	glbBool qfieldsDecode(const CGlbString& rdstr,JsonFields& jsonfields,IGlbFeatureClass* ftcls,IGlbObjectClass* objcls);
	glbBool featuresDecode(const CGlbString& rdstr,JsonFeatures& jsonfeatures,IGlbFeatureClass* ftcls,CGlbFields* fields);
	glbBool objectsDecode(const CGlbString& rdstr,JsonObjects& jsonobjects,IGlbObjectClass* objcls,CGlbFields* fields);
private:
	JsonDataset* objclsDecode(Json::Value& jsonValue);
	JsonDataset* ftclsDecode(Json::Value& jsonValue);
	JsonDataset* rasterDSetDecode(Json::Value& jsonValue);
	JsonDataset* tedDSetDecode(Json::Value& jsonValue);
	JsonDataset* drillDSetDecode(Json::Value& jsonValue);
	JsonDataset* sectionDSetDecode(Json::Value& jsonValue);
	JsonDataset* networkDSetDecode(Json::Value& jsonValue);
	void decodeFieldValue(Json::Value& jsonvalue,GLBVARIANT* var,JsonGeometries& jsongeoes);
	CGlbFeature* featureDecode(Json::Value& jsonfeature,IGlbFeatureClass* ftcls,CGlbFields* fields);
	CGlbObject*  objectDecode(Json::Value& jsonfeature,IGlbObjectClass* ftcls,CGlbFields* fields);
	void geometryDecode(Json::Value& jsongeo,JsonGeometries& jsongeoes);
	IGlbGeometry* json2point(Json::Value& jsonvalue);
	IGlbGeometry* json2mpoint(Json::Value& jsonvalue);
	IGlbGeometry* json2line(Json::Value& jsonvalue);
	IGlbGeometry* json2mline(Json::Value& jsonvalue);
	IGlbGeometry* json2polygon(Json::Value& jsonvalue);
	IGlbGeometry* json2mpolygon(Json::Value& jsonvalue);
	IGlbGeometry* json2tin(Json::Value& jsonvalue);
	IGlbGeometry* json2mtin(Json::Value& jsonvalue);
};

