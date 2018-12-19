#include "StdAfx.h"
#include "GlbCalculateNodeSizeVisitor.h"

GlbGlobe::CGlbCalculateNodeSizeVisitor::CGlbCalculateNodeSizeVisitor():
osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
	mpr_nodeMemSize(0)
{

}

GlbGlobe::CGlbCalculateNodeSizeVisitor::~CGlbCalculateNodeSizeVisitor()
{

}