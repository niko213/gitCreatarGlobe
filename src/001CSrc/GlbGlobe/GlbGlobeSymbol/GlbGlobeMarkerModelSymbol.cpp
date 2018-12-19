#include "StdAfx.h"
#include "GlbGlobeMarkerModelSymbol.h"
#include "osgDB/ReadFile"
#include "GlbString.h"
#include "GlbLog.h"
#include "osgUtil/Statistics"

// ��̬�����osgdb_**.lib��������
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
		//modelNode  = ��ʵ����������Find(datalocate )
		//CGlbGlobeInstanceManager::findInstance(datalocate)
	}
	else
	{
		/*
		       level:3,2,1,0.  3���ϸ��
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
			{// ����ǰִ���ļ������·����� ./AAA/....
				CGlbWString execDir = CGlbPath::GetExecDir();
				datalocate = execDir + datalocate.substr(1,datalocate.size());
			}
			modelNode = osgDB::readNodeFile(datalocate.ToString());
			
			if (modelNode==NULL)
			{
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"��ȡģ���ļ�(%s)ʧ��. \r\n",datalocate.c_str());
				return NULL;
			}
			//{// �ж��Ƿ���Ҫ���� kdtree ����ѡ��  2016.11.4 ml ��Ƭ������5000����kdtree�Զ�����?
				// -- ���û��kdtree���������ؽӿڵĻ������ô˴���			
			//	int verNum,faceNum;
			//	statusNode(modelNode,verNum,faceNum);
			//	if (faceNum > 5000) 
			//	{
			//		// ע��˴�һ��Ҫȥ���ļ�catch���Է�ֹ�����ڴ�����������!!!! ���� 20160112
			//		osg::ref_ptr<osgDB::Options> op = new osgDB::Options();
			//		op->setObjectCacheHint(osgDB::Options::CACHE_NONE);
			//		osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::Options::BUILD_KDTREES);
			//		modelNode = osgDB::readNodeFile(datalocate.ToString(),op.get());
			//		osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::Options::NO_PREFERENCE);
			//	}
			//}
		}
		//��modeNode ��ӵ�ʵ����������
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
