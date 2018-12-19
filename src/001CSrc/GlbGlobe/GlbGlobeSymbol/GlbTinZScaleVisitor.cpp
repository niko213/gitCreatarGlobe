#include "StdAfx.h"
#include "GlbTinZScaleVisitor.h"

using namespace GlbGlobe;

CGlbTinZScaleVisitor::CGlbTinZScaleVisitor(double zScale)
	: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	mpr_zScale = zScale;
	mpr_trans.set(0,0,0);
}


CGlbTinZScaleVisitor::~CGlbTinZScaleVisitor(void)
{
}


void CGlbTinZScaleVisitor::changeVertexCoord(osg::Geometry* geom)
{
	osg::Array::Type type = geom->getVertexArray()->getType();
	switch(type)
	{
	case osg::Array::Vec3ArrayType:
		{
			osg::ref_ptr<osg::Vec3Array> vecArray = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

			double ptx,pty,ptz;										
			for (unsigned int idx = 0 ; idx < vecArray->size(); idx++)
			{ 
				osg::Vec3 vt = vecArray->at(idx) + mpr_trans;
				g_ellipsoidModel->convertXYZToLatLongHeight(vt.x(),vt.y(),vt.z(),pty,ptx,ptz);
				ptz *= mpr_zScale;
				g_ellipsoidModel->convertLatLongHeightToXYZ(pty,ptx,ptz,ptx,pty,ptz);

				osg::Vec3 newVt = osg::Vec3(ptx,pty,ptz)-mpr_trans;
				vecArray->at(idx).set(newVt);		
			}
			geom->dirtyDisplayList();			
		}
		break;			
	case osg::Array::Vec3dArrayType:
		{
			osg::Vec3dArray* vecArray = (osg::Vec3dArray*)(geom->getVertexArray());
		}
		break;
	}		
}