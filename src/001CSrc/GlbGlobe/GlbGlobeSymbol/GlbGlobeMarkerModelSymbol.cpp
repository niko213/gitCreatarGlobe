#include "StdAfx.h"
#include "GlbGlobeMarkerModelSymbol.h"
#include "osgDB/ReadFile"
#include "GlbString.h"
#include "GlbLog.h"
#include "osgUtil/Statistics"

// 静态编译的osgdb_**.lib库这样用
//USE_OSGPLUGIN(3ds)
//#pragma comment( lib,"osgdb_3ds.lib" )  

using namespace GlbGlobe;

CGlbGlobeMarkerModelSymbol::CGlbGlobeMarkerModelSymbol(void)
{
}


CGlbGlobeMarkerModelSymbol::~CGlbGlobeMarkerModelSymbol(void)
{
}

osg::Node * CGlbGlobeMarkerModelSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo,glbInt32 level)
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbMarkerModelSymbolInfo *markerModelInfo = 
		                        static_cast<GlbMarkerModelSymbolInfo *>(renderInfo);
	if(NULL == markerModelInfo)
		return NULL;
	osg::Node *modelNode = NULL;
	CGlbFeature *feature = obj->GetFeature();
	CGlbWString datalocate = L"";

	if(markerModelInfo->locate != NULL)
		datalocate = markerModelInfo->locate->GetValue(feature);

	if (obj->IsUseInstance())
	{
		//modelNode  = 从实例化管理器Find(datalocate )
		//CGlbGlobeInstanceManager::findInstance(datalocate)
	}
	else
	{
		/*
		       level:3,2,1,0.  3是最精细的
			   3: modepath = dir/mdname
			   2:modepath = dir/2/mdname
			   1:modepath = dir/1/mdname
			   0:modepath = dir/0/mdname
			   
			   datalocate ------> level3 modepath
		*/
		CGlbWString dir      = CGlbPath::GetDir(datalocate.c_str());
		CGlbWString fname= CGlbPath::GetFileName(datalocate);
		if(dir.size() == 0)
			return NULL;
		while(level<3)
		{			
			CGlbWString lodFile = dir;
			if (dir.at(dir.size()-1)=='\\')
				lodFile +=L"%ld\\%s";
			else
				lodFile +=L"\\%ld\\%s";
			lodFile = CGlbWString::FormatString((glbWChar*)lodFile.c_str(),level,fname.c_str());			
			modelNode = osgDB::readNodeFile(lodFile.ToString());
			if(modelNode == NULL)
				level++;
			else
				break;
		}
		if(modelNode == NULL)
		{
			glbInt32 index = datalocate.find_first_of(L'.');
			if(index == 0)
			{// 处理当前执行文件的相对路径情况 ./AAA/....
				CGlbWString execDir = CGlbPath::GetExecDir();
				datalocate = execDir + datalocate.substr(1,datalocate.size());
			}
			modelNode = osgDB::readNodeFile(datalocate.ToString());
			
			if (modelNode==NULL)
			{
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取模型文件(%s)失败. \r\n",datalocate.c_str());
				return NULL;
			}
			//{// 判断是否需要创建 kdtree 加速选择  2016.11.4 ml 面片数超出5000后即做kdtree自动创建?
				// -- 如果没有kdtree树创建开关接口的话就启用此代码			
			//	int verNum,faceNum;
			//	statusNode(modelNode,verNum,faceNum);
			//	if (faceNum > 5000) 
			//	{
			//		// 注意此处一定要去除文件catch，以防止数据内存区域公用问题!!!! 马林 20160112
			//		osg::ref_ptr<osgDB::Options> op = new osgDB::Options();
			//		op->setObjectCacheHint(osgDB::Options::CACHE_NONE);
			//		osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::Options::BUILD_KDTREES);
			//		modelNode = osgDB::readNodeFile(datalocate.ToString(),op.get());
			//		osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::Options::NO_PREFERENCE);
			//	}
			//}
		}
		//把modeNode 添加到实例化管理器
		//CGlbGlobeInstanceManager::addInstance(datalocate,modeNode)
	}
	return modelNode;
}

void CGlbGlobeMarkerModelSymbol::statusNode(osg::Node* node,int& verNum,int& faceNum)
{
	osg::ref_ptr<osgUtil::StatsVisitor> sv=new osgUtil::StatsVisitor();
	sv->apply(*node);
	sv->totalUpStats();

	unsigned int unique_primitives = 0;
	osgUtil::Statistics::PrimitiveCountMap::iterator pcmitr;
	for(pcmitr =sv->_uniqueStats.GetPrimitivesBegin();
		pcmitr !=sv-> _uniqueStats.GetPrimitivesEnd();
		++pcmitr)
	{
		unique_primitives += pcmitr->second;
	}

	verNum=sv->_uniqueStats._vertexCount;
	faceNum=unique_primitives;
}
