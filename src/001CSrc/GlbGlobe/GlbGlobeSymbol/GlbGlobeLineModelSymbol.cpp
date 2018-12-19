#include "StdAfx.h"
#include "GlbGlobeLineModelSymbol.h"
#include "osgDB/ReadFile"
#include "GlbString.h"
#include "GlbLine.h"
#include "CGlbGlobe.h"
#include <osg/Switch>
#include <osg/MatrixTransform>
#include <osg/LineWidth>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include "GlbLog.h"

using namespace GlbGlobe;

CGlbGlobeLineModelSymbol::CGlbGlobeLineModelSymbol(void)
{
}


CGlbGlobeLineModelSymbol::~CGlbGlobeLineModelSymbol(void)
{
}

osg::Node *CGlbGlobeLineModelSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	GlbLineModelSymbolInfo* renderInfo = dynamic_cast<GlbLineModelSymbolInfo*>(obj->GetRenderInfo());
	if (renderInfo==NULL) return NULL;
	GlbGlobeTypeEnum globeType = obj->GetGlobe()->GetType();
	CGlbFeature* feature = obj->GetFeature();
	CGlbWString datalocate = L"";
	osg::Group* grpNode = new osg::Group;
	if (renderInfo->modelLocate)
	{
		datalocate = renderInfo->modelLocate->GetValue(feature);
		if (datalocate.length()>0)
		{
			CGlbWString ext = CGlbPath::GetExtentName(datalocate);
			if (ext == L"ini" || ext == L"INI")
			{
				return DrawLageImage(geo,datalocate,globeType==GLB_GLOBETYPE_GLOBE ? true : false);
			}
			
			glbInt32 index = datalocate.find_first_of(L'.');
			if(index == 0)
			{// 处理当前执行文件的相对路径情况 ./AAA/....
				CGlbWString execDir = CGlbPath::GetExecDir();
				datalocate = execDir + datalocate.substr(1,datalocate.size());
			}
			osg::ref_ptr<osg::Node> modelNode = osgDB::readNodeFile(datalocate.ToString().c_str());
			if (modelNode)
				grpNode->addChild(modelNode);
			else //if (datalocate.length()>0)
			{
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取线模型文件(%s)失败. \r\n",datalocate.c_str());
			}
		}
	}

	if (renderInfo->isRenderLine)
	{
		glbInt32 color = renderInfo->color->GetValue(feature);		
		glbInt32 opacity = renderInfo->opacity->GetValue(feature);
		glbInt32 width = renderInfo->width->GetValue(feature);
		CGlbLine* line = dynamic_cast<CGlbLine*>(geo);
		if (line)
		{
			glbInt32 ptCnt = line->GetCount();
			if (ptCnt>1)
			{				
				osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
				grpNode->addChild(trans);
				osg::ref_ptr<osg::Geode> blGeode = new osg::Geode;
				trans->addChild(blGeode.get());
				osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
				blGeode->addDrawable(geom.get());			

				glbDouble ptx,pty,ptz;
				line->GetPoint(0,&ptx,&pty,&ptz);
				if (GLB_GLOBETYPE_GLOBE==globeType)						
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

				osg::Matrix m;
				osg::Vec3d origVec(ptx,pty,ptz);
				m.setTrans(origVec);
				trans->setMatrix(m);

				osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
				osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;
				for (glbInt32 k = 0; k < ptCnt; k++)
				{
					line->GetPoint(k,&ptx,&pty,&ptz);						
					if (GLB_GLOBETYPE_GLOBE==globeType)						
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

					vtArray->push_back(osg::Vec3(ptx,pty,ptz)-origVec);
				}

				geom->setVertexArray(vtArray.get());
				osg::Vec4 lnColor = GetColor(color);
				lnColor.set(lnColor.r(),lnColor.g(),lnColor.b(),opacity/100.0f);
				clrArray->push_back(lnColor);
				geom->setColorArray(clrArray);
				geom->setColorBinding(osg::Geometry::BIND_OVERALL);

				geom->addPrimitiveSet(
					new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vtArray->size()));

				//线宽
				osg::LineWidth *lineWidth = new osg::LineWidth;
				lineWidth->setWidth(width);
				osg::StateSet* ss = grpNode->getOrCreateStateSet();
				ss->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);

				// 关闭光照
				//ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
				//混合是否启动
				if (opacity<100)
				{// 启用混合
					ss->setMode(GL_BLEND, osg::StateAttribute::ON);			
					if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
						ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );					
				}
				else
				{// 禁用混合						
					ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);					
					if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
						ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
				}
			}
		}
	}

	if (grpNode->getNumChildren()>0)
	{
		return grpNode;
	}
	return NULL;
}
//#include <fstream>
osg::Node *CGlbGlobeLineModelSymbol::DrawLageImage(IGlbGeometry *geo, CGlbWString dataPath, bool isGlobe)
{
	CGlbLine* line = dynamic_cast<CGlbLine*>(geo);
	if (line==NULL) return NULL;
	glbInt32 ptCnt = line->GetCount();
	if (ptCnt<2) return NULL;

	// 读取.ini文件中所有的图片信息	
	double secDepth = 100.0;
	std::vector<osg::ref_ptr<osg::Image>> _pImages;	
	int _totalImageWidth = 0;
	{// 读取dataPath中的图片路径信息和线剖面的深度信息
		FILE *fp=fopen(dataPath.ToString().c_str(),"r");		
		if (fp)
		{
			fscanf(fp,"%lf\n",&secDepth);

			int code = 0;
			char s[128];
			while(fscanf(fp,"%s\n",s) != EOF)
			{
				osg::ref_ptr<osg::Image> _img = osgDB::readImageFile(s);
				if (_img)
				{
					_pImages.push_back(_img);
					_totalImageWidth += _img->s();
				}
			}
			fclose(fp);
		}
	}	
	int cnt = _pImages.size();
	if (cnt < 0) return NULL;

	double ptx,pty,ptz;
	double ptx0,pty0,ptz0;
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	line->GetExtent()->GetCenter(&ptx,&pty,&ptz);
	if (isGlobe)						
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

	osg::Matrix m;
	osg::Vec3d origVec(ptx,pty,ptz);
	m.setTrans(origVec);
	trans->setMatrix(m);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	trans->addChild(geode);

	double totalLength = 0.0;
	std::vector<double> segmentsLength;
	segmentsLength.push_back(0.0);
	for (glbInt32 k = 1; k < ptCnt; k++)
	{
		line->GetPoint(k-1,&ptx,&pty,&ptz);
		if (isGlobe)						
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
		line->GetPoint(k,&ptx0,&pty0,&ptz0);	
		if (isGlobe)						
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty0),osg::DegreesToRadians(ptx0),ptz0,ptx0,pty0,ptz0);

		double len = sqrt((ptx-ptx0)*(ptx-ptx0)+(pty-pty0)*(pty-pty0)+(ptz-ptz0)*(ptz-ptz0));		
		totalLength += len;
		segmentsLength.push_back(totalLength);
	}		

	double cur_wid = 0;
	osg::Vec3 v0, v1; 
	for (int idx = 0; idx < cnt; idx++)
	{
		osg::ref_ptr<osg::Image> img = _pImages.at(idx);

		double c = cur_wid / (double)_totalImageWidth;
		int prePointIdx0,prePointIdx1;
		v0 = GetCoordByLinePercent(geo,c,segmentsLength,totalLength,prePointIdx0,isGlobe);

		cur_wid += img->s();
		c = cur_wid / (double)_totalImageWidth;	
		v1 = GetCoordByLinePercent(geo,c,segmentsLength,totalLength,prePointIdx1,isGlobe);

		double segV0_x,segV0_y,segV0_z;
		segV0_x = v0.x();
		segV0_y = v0.y();
		segV0_z = v0.z();
		osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec2Array> texcoordArray = new osg::Vec2Array();
		if (isGlobe)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(v0.y()),osg::DegreesToRadians(v0.x()),v0.z(),ptx,pty,ptz);
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(v0.y()),osg::DegreesToRadians(v0.x()),v0.z(),segV0_x,segV0_y,segV0_z);

			vtArray->push_back(osg::Vec3(ptx,pty,ptz) - origVec);
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(v0.y()),osg::DegreesToRadians(v0.x()),v0.z()-secDepth,ptx,pty,ptz);
			vtArray->push_back(osg::Vec3(ptx,pty,ptz) - origVec);
		}
		else
		{
			vtArray->push_back(v0 - origVec);
			vtArray->push_back(v0+osg::Vec3(0,0,-secDepth) - origVec);
		}
		texcoordArray->push_back(osg::Vec2(0,1));
		texcoordArray->push_back(osg::Vec2(0,0));		

		double segLength = img->s() * totalLength / _totalImageWidth;
		double accumulateLen = 0.0;
		if (prePointIdx1 > prePointIdx0)
		{
			for (int kk = prePointIdx0+1; kk <= prePointIdx1; kk++)
			{
				double cc;

				line->GetPoint(kk,&ptx,&pty,&ptz);
				if (isGlobe)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx0,pty0,ptz0);
					
					double len = sqrt((segV0_x-ptx0)*(segV0_x-ptx0)+(segV0_y-pty0)*(segV0_y-pty0)+(segV0_z-ptz0)*(segV0_z-ptz0));
					accumulateLen += len;
					cc = accumulateLen / segLength;
					segV0_x = ptx0; 
					segV0_y = pty0;
					segV0_z = ptz0;

					vtArray->push_back(osg::Vec3(ptx0,pty0,ptz0) - origVec);
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz-secDepth,ptx0,pty0,ptz0);
					vtArray->push_back(osg::Vec3(ptx0,pty0,ptz0) - origVec);
				}
				else
				{
					double len = sqrt((segV0_x-ptx)*(segV0_x-ptx)+(segV0_y-pty)*(segV0_y-pty)+(segV0_z-ptz)*(segV0_z-ptz));
					accumulateLen += len;
					cc = accumulateLen / segLength;
					segV0_x = ptx; 
					segV0_y = pty;
					segV0_z = ptz;

					vtArray->push_back(osg::Vec3(ptx,pty,ptz) - origVec);
					vtArray->push_back(osg::Vec3(ptx,pty,ptz-secDepth) - origVec);
				}
				
				texcoordArray->push_back(osg::Vec2(cc,1));
				texcoordArray->push_back(osg::Vec2(cc,0));
			}
		}
		if (isGlobe)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(v1.y()),osg::DegreesToRadians(v1.x()),v1.z(),ptx,pty,ptz);
			vtArray->push_back(osg::Vec3(ptx,pty,ptz) - origVec);
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(v1.y()),osg::DegreesToRadians(v1.x()),v1.z()-secDepth,ptx,pty,ptz);
			vtArray->push_back(osg::Vec3(ptx,pty,ptz) - origVec);
		}
		else
		{
			vtArray->push_back(v1 - origVec);
			vtArray->push_back(v1+osg::Vec3(0,0,-secDepth) - origVec);
		}
		texcoordArray->push_back(osg::Vec2(1,1));
		texcoordArray->push_back(osg::Vec2(1,0));

		// 生成geometry
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		geom->setVertexArray(vtArray);
		geom->setTexCoordArray(0,texcoordArray);

		osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;		
		tex->setImage(img.get());
		tex->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR_MIPMAP_LINEAR);
		tex->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);		
		tex->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
		tex->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);
		tex->setMaxAnisotropy(8.0);
		tex->setResizeNonPowerOfTwoHint(true);
		geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,tex.get());	

		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,vtArray->size()));

		geode->addDrawable(geom);
	}
	// 关闭光照
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

	return trans.release();
}

osg::Vec3 CGlbGlobeLineModelSymbol::GetCoordByLinePercent(IGlbGeometry *geo, double per, std::vector<double> segLen, double totallength, int& prePointIdx, bool isGlobe)
{
	CGlbLine* ln = dynamic_cast<CGlbLine*>(geo);	
	
	double cur = totallength * per;
	int cnt = segLen.size();
	
	int preIdx = -1;
	int nextIdx = -1;
	for (int kk = 0; kk < cnt; kk++)
	{
		double t = segLen.at(kk) / (double)totallength;
		if (t <= per)
			preIdx = kk;
		if (t >= per)
		{
			nextIdx = kk;
			break;
		}
	}
	if (preIdx < 0 || nextIdx < 0)
		int _error = 1;

	if (nextIdx == preIdx)
	{
		if (nextIdx==segLen.size()-1)
			preIdx = nextIdx-1;
		else
			nextIdx = preIdx+1;
	}

	prePointIdx = preIdx;

	double c = (cur - segLen[preIdx]) / (segLen[nextIdx] - segLen[preIdx]) ;

	double ptx,pty,ptz, ptx2,pty2,ptz2;
	ln->GetPoint(preIdx,&ptx,&pty,&ptz);
	ln->GetPoint(nextIdx,&ptx2,&pty2,&ptz2);
	if (isGlobe)
	{
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty2),osg::DegreesToRadians(ptx2),ptz2,ptx2,pty2,ptz2);
	}

	ptx = ptx + (ptx2 - ptx) * c;
	pty = pty + (pty2 - pty) * c;
	ptz = ptz + (ptz2 - ptz) * c;
	if (isGlobe)
	{
		g_ellipsoidModel->convertXYZToLatLongHeight(ptx,pty,ptz,pty,ptx,ptz);
		ptx = osg::RadiansToDegrees(ptx);
		pty = osg::RadiansToDegrees(pty);
	}
	osg::Vec3 vt(ptx,pty,ptz);
	return vt;
}