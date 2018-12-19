#include "StdAfx.h"
#include "GlbCalculateBoundBoxVisitor.h"
using namespace GlbGlobe;

CGlbCalculateBoundBoxVisitor::CGlbCalculateBoundBoxVisitor(void)
	: NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
}


CGlbCalculateBoundBoxVisitor::~CGlbCalculateBoundBoxVisitor(void)
{
}
